#define _GNU_SOURCE
#define BUF_LENGHT 128

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "headers/filesIO.h"
#include "headers/segnali.h"
#include "headers/thpool.h"

static config_t* config = NULL;
volatile sig_atomic_t termina;
volatile sig_atomic_t shutdownNow = 0;

static int sfd;
pthread_t tid_dispatcher;
static threadpool thpool;

char buf[BUF_LENGHT];

void cleanup();

void printConfig(config_t* c){
    printf("nworkers: %d\n", c->nworkers);
    printf("maxnfiles: %d\n", c->maxnfiles);
    printf("maxcapacity: %d\n", c->maxcapacity);
    printf("socketname: %s\n", c->socketname);
}

void readCfgFile(){
    int fd_cfg = openFile("server/config.txt", O_RDONLY,NULL);
    char line[512];
    FILE* fp_cfg = fdopen(fd_cfg, "r");
    MALLOC(config, sizeof(config));

    while(fgets(line, sizeof(line), fp_cfg)){
        if(line[0] == '#')
            continue;
        parseLine(line, config);
    }

    fclose(fp_cfg);
}

void parseLine(char* line, config_t * config){
    char* save;
    int len;
    
    char* key;
    char* value;

    char* token = strtok_r(line, ":", &save);
    len = strlen(token) + 1;
    MALLOC(key, len);
    strncpy(key, token, len);
    key[len] = '\0';
    token = strtok_r(NULL, ":", &save);
    if(token == NULL || strncmp(token, "", 1) == 0 || strncmp(token, "\n", 1) == 0){
    }else{
        len = strlen(token) + 1;
        MALLOC(value, len);
        strncpy(value, token, len);
        value[len] = '\0';
        insertConfig(key, value, config);
    }

    free(key);
    free(value);
}

void insertConfig(const char *key, const char *value, config_t *config){
    if(strncmp(key, "nworkers", strlen(key)) == 0){
        config->nworkers = strtol(value, NULL, 10);
    }else if(strncmp(key, "maxnfiles", strlen(key)) == 0){
        config->maxnfiles = strtol(value, NULL, 10);
    }else if(strncmp(key, "maxcapacity", strlen(key)) == 0){
        config->maxcapacity = strtol(value, NULL, 10);
    }else if(strncmp(key, "socketname", strlen(key)) == 0){
        MALLOC(config->socketname, strlen(value));
        strncpy(config->socketname,value, strlen(value));
    }
}

void cleanup(){
    if(!shutdownNow)
        thpool_destroy(thpool);
    printf("threadpool destroyed");
    //close(sfd);
    removeFile(config->socketname);
    free(config->socketname);
    free(config);
}

void handlerSIGINT_SIGQUIT(){
    shutdownNow = 1;
    termina = 1;
    ec_meno1(shutdown(sfd, SHUT_RDWR), "shutdown");
}

void handlerSIGHUP(){
    termina = 1;
    ec_meno1(shutdown(sfd, SHUT_RDWR), "shutdown");
}

void worker(void* args){
    int fd = (int) args;

    int finish = 0;
    // ricevuto fd, a questo punto il thread client manda le operazioni una alla volta
    while(!finish){
        //legge operazione mandata dal client
        ec_meno1(read(fd, buf, BUF_LENGHT), "error");
        if(!strcmp(buf, "exit")){
            printf("Operation received: %s\n", buf);
            fflush(stdout);
            ec_meno1(write(fd, "exited", 7), "error");
            finish = 1;
            continue;
        }
        printf("Operation received: %s\n", buf);
        fflush(stdout);
        // ec_meno1(read(fd, buf, BUF_LENGHT),"error" );
        // printf("Suboperation received: %s\n", buf);
        // fflush(stdout);
        while(strcmp(buf, "NOARGS") != 0) {  
            ec_meno1(read(fd, buf, BUF_LENGHT), "error");
            printf("Suboperation received: %s\n", buf);
            fflush(stdout);
            ec_meno1(write(fd, "ok", 3), "error"); // like 200 OK
            sleep(0.5);
        }

        // TODO here check the operation and execute it. (API)

        //invia esito al client
        ec_meno1(write(fd, "ok", 3),"error"); // like 200 OK
        fflush(stdout);
        
    }
    close(fd);
    printf("Worker finished\n");
}

void* dispatcher(void* args){

    sigset_t set;
    struct sigaction sa;
    ec_meno1(sigfillset(&set), "sigfillset");
    ec_meno1(pthread_sigmask(SIG_SETMASK, &set, NULL), "");

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handlerSIGINT_SIGQUIT;
    ec_meno1(sigaction(SIGINT, &sa, NULL), "sigaction");
    ec_meno1(sigaction(SIGQUIT, &sa, NULL), "sigaction");
    sa.sa_handler = handlerSIGHUP;
    ec_meno1(sigaction(SIGHUP, &sa, NULL), "sigaction");

    ec_meno1( sigemptyset(&set), "");
    ec_meno1(pthread_sigmask(SIG_SETMASK,&set,NULL), "");

    struct sockaddr_un skta;
    strncpy(skta.sun_path, config->socketname, UNIX_PATH_MAX);
    skta.sun_family = AF_UNIX;
    int fd_c;
    
    ec_meno1(sfd=socket(AF_UNIX, SOCK_STREAM, 0), "socket");
    ec_meno1(bind(sfd, (struct sockaddr *) &skta, sizeof(skta)), "bind");
    ec_meno1(listen(sfd, SOMAXCONN), "listen");

    //int client_counter = 0;
    thpool = thpool_init(config->nworkers);
    
    while(!termina){
        printf("In attesa di connessioni su %s...\n", config->socketname);
        fflush(stdout);
        if((fd_c = accept(sfd,NULL,0)) == -1){
            break;
        }
        if(!termina)
            thpool_add_work(thpool, worker, (void*)fd_c);
    }
    //thpool_wait(thpool);
    printf("Dispatcher ended\n");
    pthread_exit(0);
}



int main(int argc, char const *argv[]){
    // 1) legge file di conf e lo salva in una sd (config_t)
    readCfgFile();
    printConfig(config);

    // 2) avvia un thread che rimane in attesa di nuove connessioni da parte del client e le manda al dispatcher
    ec_meno1(pthread_create(&tid_dispatcher, NULL, &dispatcher, NULL), "pthread_create");
    pthread_join(tid_dispatcher, NULL);
    cleanup();
    return 0;
}

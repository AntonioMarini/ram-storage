#define _GNU_SOURCE

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

void cleanup();

void printConfig(config_t* c){
    printf("nworkers: %d\n", c->nworkers);
    printf("maxnfiles: %d\n", c->maxnfiles);
    printf("maxcapacity: %d\n", c->maxcapacity);
    printf("socketname: %s\n", c->socketname);
}

int removeFile(char* filename){
    int err;
    if((err = remove(filename)) == -1){
        perror("remove");
        return -1;
    }
    return 1;
}

int openFile(char* filename,int mode, void (*cleanup) ()){
    int fd;
    if((fd = open(filename, mode)) == -1){
        perror("open");
        if(cleanup != NULL){
            cleanup();
        }else{
            exit(1);
        }
    }
    return fd;
}

int closeFile(int fd){
    if((fd = close(fd)) == -1){perror("open");exit(1);}
    return fd;
}

int readFile(int fd_in, char* buf, int n, FILE* fd_out){
    int lung;
    while((lung = read(fd_in, buf, n)) > 0){
        fprintf(fd_out, buf, n);
    }
    if(lung == -1){
        perror("read");
        exit(1);
    }
    return 1;
}

void readCfgFile(int fd_cfg){
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
    //thpool_destroy(thpool);
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
    //int fd = (int) args;
    int finish = 0;
    // ricevuto fd, a questo punto il thread client manda le operazioni una alla volta
    while(!finish){
        //legge operazione mandata dal client
        //esegue operazione
        sleep(5);
        printf("Worker finished\n");
        finish = 1;
    }
}

void* dispatcher(void* args){

    sigset_t set;
    struct sigaction sa;

    ec_meno1(sigfillset(&set), "sigfillset");
    ec_meno1(pthread_sigmask(SIG_SETMASK, &set, NULL), "");

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handlerSIGINT;
    ec_meno1(sigaction(SIGINT, &sa, NULL), "sigaction");

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
    int fd_cfg = openFile("server/config.txt", O_RDONLY,NULL);
    readCfgFile(fd_cfg);

    // 2) avvia un thread che rimane in attesa di nuove connessioni da parte del client e le manda al dispatcher
    ec_meno1(pthread_create(&tid_dispatcher, NULL, &dispatcher, NULL), "pthread_create");

    if(!shutdownNow)
        pthread_join(tid_dispatcher, NULL);
    cleanup();
    return 0;
}

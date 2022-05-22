#define BUF_LENGHT 128

#include "headers/consts.h"
#include "headers/opqueue.h"
#include "headers/api.h"

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>

// int openConnection(const char* sockname, int msec, const struct timespec abstime){
// }
static OpQueue opQueue = NULL; // head of the operations queue
int fd_skt;
char* buf;

/**
 * @brief Checks if a given option character is valid
 * 
 * @param arg 
 * @return char 
 */
char isOption(const char* arg){
    // :hf:wWDrRdtlucp
    //check if has dash

    if(arg[0] == '\0')
        return '\0';

    char op = arg[0];

    char* opts = "hfwWDrRdtlucp";
    char cur = opts[0];
    int i = 0;

    while(cur != '\0'){
        if(cur == op)
            return op;
        cur = opts[++i];
    }
    return '\0';
}

/**
 * @brief from an option string tokenize it to take all the sub options
 * 
 * @param option 
 */
char** takeSubopts(char* option){
    char suboptsstr[strlen(option)];

    char** subopts;
    MALLOC(subopts, 64, char*);

    strncpy(suboptsstr, &option[1], strlen(option));
    suboptsstr[strlen(suboptsstr)] = '\0';
    
    //printf("suboptions are: %s\n" , suboptsstr);
    const char suboptdelim[2] = ",";
    char* token;
    char* save = NULL;
    
    int i = 0;
    int len;
    token = strtok_r(suboptsstr, suboptdelim, &save);

    while(token != NULL){
        trimwhitespace(token);
        len = strlen(token) + 1;
        MALLOC(subopts[i], len, char);
        strncpy(subopts[i], token, len);
        token = strtok_r(NULL, suboptdelim, &save);
        i++;
    }
    subopts[i]=NULL;

    return subopts;
}

/**
 * @brief Populates queue with the given string of args
 * 
 * @param argstr 
 * @param queue 
 */
void tokenizeArgs(char* argstr, OpQueue* queue){
    char* token;
    const char optdelim[2] = "-";
    char* save = NULL;
    char op;

    token = strtok_r(argstr, optdelim, &save);

    while (token != NULL){
        //printf("%s\n", token);
        
        if(token != NULL){
            if((op = isOption(token)) == '\0'){
                printf("not valid option");
                exit(1);
            }
            char** subopts = takeSubopts(token);
            OpNode* opNode = createOpNode(op, subopts);

            if((opNode->op == PRINT && existsOp(opQueue, PRINT)) || (opNode->op == CONNECT && existsOp(opQueue, CONNECT)))
                printf("opnode is not valid\n");
            else{  
                pushOpNode(&opQueue, opNode);
            }
        }
        token = strtok_r(NULL, optdelim, &save);
    }
}

/**
 * @brief Concat all the args in a single string
 * 
 * @param argc 
 * @param argv 
 * @param argstr 
 */
void scanArgs(int argc, const char** argv, char* argstr){
    // scorro la lista di args
    for(int i = 1; i < argc; i++){
        strncat(argstr, argv[i], 2048);
        strncat(argstr, " ", 2048);
    }
}

void cleanup(){
    free(opQueue);
}

void sendCommand(char* op, char** subopts){
    if (op == NULL)
        return;

    ec_meno1(write(fd_skt, op, strlen(op) + 1), "error");
    sleep(0.5);

    if(subopts != NULL){
        int i = 1;
        ec_meno1(write(fd_skt, subopts[0], sizeof(subopts[i]) + 1), "error");
        ec_meno1(read(fd_skt, buf, BUF_LENGHT), "error");
        while(subopts[i] != NULL){
            ec_meno1(write(fd_skt, subopts[i], sizeof(subopts[i]) + 1), "error");
            //sleep(1);
        fflush(stdout);
            ec_meno1(read(fd_skt, buf, BUF_LENGHT), "error");
            i++;
        }  
    }       
}

/**
 * @brief 
 * 
 * @param opNode 
 */
void executeOp(OpNode* opNode){
    // per ogni operazione aspetto l'esito
    //printOpNode(opNode);
    buf = malloc(BUF_LENGHT);
    switch(opNode->op){
        case CONNECT:{
            printf("Set the socket name to connect to as %s\n", opNode->subopts[0]);
            struct timespec ts;
            ts.tv_sec = 3;
            if(openConnection(opNode->subopts[0], 1000, ts) == 0)
                printf("Connection failed\n");
            freeOpNode(opNode);
            return;
        }case WRITEDIR:{
            printf("Writing the folder in the server...\n");
            sendCommand("w", opNode->subopts);
            break;
        }case WRITEF:{
            printf("Writing the files in the server...\n");
            sendCommand("W", opNode->subopts);
            break;
        }case CLIENTDIR:{ 
            printf("Setting the client directory name...\n");
            sendCommand("D", opNode->subopts);
            printf("Esito operazione: %s\n", buf);
            break;
        }case READN:{ 
            printf("Reading the files from the server...\n");
            sendCommand("r", opNode->subopts);
            break;
        }case READRAND:{ 
            printf("Reading random files from the server...\n");
            write(fd_skt, "R", 2);
            break;
        }case DIRNAME:{
            printf("Setting the dirname where to save files...\n");
            sendCommand("d", opNode->subopts);
            break;
        }case LOCKN:{ 
            printf("Locking the files...\n");
            sendCommand("l", opNode->subopts);
            break;
        }case UNLOCKN:{
            printf("Unlocking the files...\n");
            sendCommand("u", opNode->subopts);
            break;
        }case REMOVEN:{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            printf("Removing the files from the server...\n");
            sendCommand("c", opNode->subopts);
            break;
        }
    }

    write(fd_skt, "NOARGS", 7);
    sleep(1);

    if(read(fd_skt, buf, sizeof(buf)) != -1) //leggi esito
        printf("Esito operazione: %s\n", buf);
    else{perror("read"); freeOpNode(opNode); return;}

    freeOpNode(opNode);
}

int main(int argc, char const *argv[])
{
    // 1) scan the args and create an operation queue
    char argstr[2048] = "";
    scanArgs(argc, argv, argstr);
    tokenizeArgs(argstr, &opQueue);

    //2) validate the opqueue
    if(!validateOpQueue(opQueue)){
        printf("Error: Invalid args");
        return 1;
    }

    // 3) execute the operation queue
    while(!isEmpty(opQueue)){
        sleep(1); // TODO check the seconds interval in the specific argument
        OpNode* opNode = popOpNode(&opQueue);
        executeOp(opNode);
    }

    write(fd_skt, "exit", 5);

    // 4) cleanup memory and finish
    cleanup();
    return 0;
}

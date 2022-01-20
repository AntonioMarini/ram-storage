#include "headers/consts.h"
#include "headers/opqueue.h"

#include <getopt.h>
#include <string.h>

// int openConnection(const char* sockname, int msec, const struct timespec abstime){
// }

static OpQueue opQueue = NULL; // head of the operations queue

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
        //remove_spaces(token, token);
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

/**
 * @brief 
 * 
 * @param opNode 
 */
void executeOp(OpNode* opNode){
    switch(opNode->op){
        case CONNECT:{
            printf("Setting the socket name to connect to...\n");
            break;
        }case WRITEDIR:{
            printf("Writing the folder in the server...\n");
            break;
        }case WRITEF:{
            printf("Writing the files in the server...\n");
            break;
        }case CLIENTDIR:{ 
            printf("Setting the client directory name...\n");
            break;
        }case READN:{ 
            printf("Reading the files from the server...\n");
            break;
        }case READRAND:{ 
            printf("Reading random files from the server...\n");
            break;
        }case DIRNAME:{
            printf("Setting the dirname where to save files...\n");
            break;
        }case TIME:{
            printf("Setting the time to execute commands...\n");
            break;
        }case LOCKN:{ 
            printf("Locking the files...\n");
            break;
        }case UNLOCKN:{
            printf("Unlocking the files...\n");
            break;
        }case REMOVEN:{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            printf("Removing the files from the server...\n");
            break;
        }case PRINT:{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            printf("Enabling the debug mode...\n");
            break;
        }
    }

    freeOpNode(opNode);
}

void executeOpQueue(){

}

int main(int argc, char const *argv[])
{
    // 1) scan the args and create an operation queue
    char argstr[2048] = "";
    scanArgs(argc, argv, argstr);
    tokenizeArgs(argstr, &opQueue);

    //2) set the environment to execute the queue
    if(!validateOpQueue(opQueue)){
        printf("Error: Invalid args");
        return 1;
    }

    // 3) execute the operation queue
    while(!isEmpty(opQueue)){
        OpNode* opNode = popOpNode(&opQueue);
        executeOp(opNode);
    }

    // 4) cleanup memory and finish
    cleanup();
    return 0;
}

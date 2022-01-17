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
            pushOpNode(&opQueue, opNode);
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

int main(int argc, char const *argv[])
{
    char argstr[2048] = "";
    scanArgs(argc, argv, argstr);
    tokenizeArgs(argstr, &opQueue);

    OpNode* del = popOpNode(&opQueue);
    printOpQueue(opQueue);

    printf("\n\ndeleted node: ");
    printOpNode(del);

    freeOpNode(del);
    freeOpQueue(opQueue);
    return 0;
}

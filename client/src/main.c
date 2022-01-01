#include "headers/api.h"
#include "headers/consts.h"
#include <stdio.h>
#include <stdlib.h>

int openConnection(const char* sockname, int msec, const struct timespec abstime){
    // not implemented
    return 0;
}

int closeConnection(const char* sockname){
    // not implemented
    return 0;
}

/* @returns 1 if exit command, otherwise 0
*/
int exitCommand(char* cmd){
    return (strncmp(cmd, "exit", 4) == 0);
}

void readCmdLine(char** argv, int* argc){ 
    *argc = 0;
    int len;
    char* line = (char*) malloc(sizeof(char) * MAXLINE);
    //argv = (char**) malloc(sizeof(char*) * MAXARGS);
    
    ec_meno1(read(STDIN_FILENO, line, MAXLINE),"read");

    char* token = strtok(line, " ");
    while (token != NULL) {
        len = strlen(token) + 1;       
        MALLOC(argv[*argc],len);
        strncpy(argv[*argc], token, len);
        token = strtok(NULL, " ");
        (*argc)++;
    }

    free(line);
    argv[*argc] = NULL;
}

void clearArgs(char** argv, int argc){
    for(int i = 0; i < argc; i++){
        free(argv[i]);
    }
}

void typecursor(){
    printf("> ");
    fflush(stdout); 
}

int main()
{
    printf("Welcome to ram-storage\n");
    printf("Please connect to a ram-storage server to begin.\n");
    printf("Use -h to see all available commands\n\n");

    char *argv[MAXARGS];
    int argc;

    while(1){
        typecursor();
        readCmdLine(argv, &argc);
        if(argv[0] == NULL){
            exit(1);
        }
        if(exitCommand(argv[0])){
            break;
        }
        // execute
        clearArgs(argv, argc);
    }
    clearArgs(argv, argc);

    printf("Bye!\n");
    return 0;
}

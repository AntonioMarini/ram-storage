#include "headers/filesIO.h"

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
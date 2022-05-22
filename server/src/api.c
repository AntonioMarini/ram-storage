#include "headers/api.h"
#include "headers/consts.h"
#include <errno.h>
#include <unistd.h>

#define UNIX_PATH_MAX 108 /* man 7 unix */
#define BUF_LENGHT 128

static int fd_skt;
char buf[BUF_LENGHT];

int closeConnection(const char* sockname){
    //bisogno del file descriptor
    //manda exit nel buffer
    return 0;
}

int openFile(const char* pathname, int flags){
    return 0;
}

int readNFiles(int N, const char* dirname){
    return 0;
}

int writeFile(const char* pathname, const char* dirname){
    return 0;
}

int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname){
    return 0;
}

int lockFile(const char* pathname){
    return 0;
}

int unlockFile(const char* pathname){
    return 0;
}

int closeFile(const char* pathname){
    return 0;
}

int removeFile(const char* pathname){
    return 0;
}
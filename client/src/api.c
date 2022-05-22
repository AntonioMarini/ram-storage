#include "headers/api.h"
#include "headers/consts.h"
#include <errno.h>
#include <unistd.h>

#define UNIX_PATH_MAX 108 /* man 7 unix */
#define BUF_LENGHT 128

int fd_skt;
char buf[BUF_LENGHT];

int openConnection(const char* sockname, int msec, const struct timespec abstime){
    struct sockaddr_un sa;
    strncpy(sa.sun_path, sockname,UNIX_PATH_MAX);
    sa.sun_family=AF_UNIX;
    ec_meno1(fd_skt = socket(AF_UNIX, SOCK_STREAM, 0), "");
    printf("socket aperto\n");
    time_t timePassed = 0;
    while(connect(fd_skt, (struct sockaddr*) &sa, sizeof(sa)) == -1 && timePassed != abstime.tv_sec){
        if(errno == ENOENT){
            sleep(msec / 1000);
            timePassed++;
        }    
        else return 0;
    }

    if(timePassed==abstime.tv_sec){
        printf("Timeout, Connection failed");
        close(fd_skt);
        return 0;
    }

    return 1;
}

int closeConnection(const char* sockname){
    //bisogno del file descriptor
    //manda exit nel buffer
    return 0;
}
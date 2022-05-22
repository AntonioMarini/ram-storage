#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>


#define MALLOC(ptr, sz, type)			   \
    if ((ptr=(typeof(ptr))malloc(sz * sizeof(type))) == NULL) { \
	perror("malloc");		   \
	exit(EXIT_FAILURE);		   \
    }

#define ec_meno1(cmd, msg) \
    if((cmd) == -1) {\
        perror("msg");\
        exit(EXIT_FAILURE);\
    } 

#define MAXLINE 1024
#define MAXARGS 128

#define print printf("qui");
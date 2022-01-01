#define MALLOC(ptr, sz)			   \
    if ((ptr=(char*)malloc(sz)) == NULL) { \
	perror("malloc");		   \
	exit(EXIT_FAILURE);		   \
    }

#define ec_meno1(cmd, msg) \
    if(cmd == -1) {\
        perror("msg");\
        exit(EXIT_FAILURE);\
    } 

#define MAXLINE 1024
#define MAXARGS 128
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UNIX_PATH_MAX 108

#define MALLOC(ptr, sz)			   \
    if ((ptr=(typeof(ptr))malloc(sz * sizeof(typeof(ptr)))) == NULL) { \
	perror("malloc");		   \
	exit(EXIT_FAILURE);		   \
    }

#define ec_meno1(cmd, msg) \
    if((cmd) == -1) {\
        perror("msg");\
        exit(EXIT_FAILURE);\
    } 

typedef struct config_file {
    unsigned int nworkers;
    unsigned int maxcapacity;
    unsigned int maxnfiles;
    char* socketname;
} config_t;

/** 
 * @brief  
 * @return fd if successful, -1 if not
*/
void insertConfig(const char *key, const char *value, config_t *config);

/**
 * @brief 
 * 
 * @param filename the name of the file
 * @param cleanup the function to be executed when there is an error
 * 
 * @return fd if successful, -1 if not
 */
int openFile(char* , int,void (*) ());

/**
 * @brief Removes the file at the specified path
 * 
 * @return int 
 */
int removeFile(char* filename);

/**
 * @brief reads a line from the config file and parse it to the config data
 * 
 * @param line 
 * @param config 
 */
void parseLine(char* , config_t * );
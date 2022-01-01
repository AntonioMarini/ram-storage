#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>     
#include <errno.h> 
#include <sys/wait.h>  
#include <sys/types.h> 

/*  
    Open an AF_UNIX connection to the socketname file 
    Every msec. The client tries to connect until 
    abstime expires.
    @returns 0 on success, -1 on failure, eventually sets errno
*/
int openConnection(const char* sockname, int msec, const struct timespec abstime);

/*  
    Close the AF_UNIX connection to the socketname file.
    @returns 0 on success, -1 on failure, eventually sets errno
*/
int closeConnection(const char* sockname);
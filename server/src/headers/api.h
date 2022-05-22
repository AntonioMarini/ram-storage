#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int closeConnection(const char* sockname);

int openFile(const char* pathname, int flags);

int readNFiles(int N, const char* dirname);

int writeFile(const char* pathname, const char* dirname);

int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname);

int lockFile(const char* pathname);

int unlockFile(const char* pathname);

int closeFile(const char* pathname);

int removeFile(const char* pathname);
#include <stdlib.h>

enum ops {
    CONNECT,
    WRITEDIR,
    WRITEF,
    CLIENTDIR,
    READN,
    READRAND,
    DIRNAME,
    TIME,
    LOCKN,
    UNLOCKN,
    REMOVEN,
    PRINT
};


typedef struct OpNode {
    enum ops op;
    char** subopts;
    struct OpNode* next;
} OpNode;
typedef OpNode* OpQueue;

/**
 * @brief Create a Op node
 * 
 * @param op 
 * @param subopts 
 * @return OpNode* 
 */
OpNode* createOpNode(char op, char** subopts);

/**
 * @brief Inserts an OpNode on the tail of the given OpQueue 
 * 
 * @param opQueue 
 * @param node 
 */
void pushOpNode(OpQueue* opQueue, OpNode* node);

/**
 * @brief Pop the first node of the queue
 * @param opQueue 
 */
OpNode* popOpNode(OpQueue* opQueue);

/**
 * @brief Prints in stdout the given OpNode (used for debug purposes)
 * 
 * @param node 
 */
void printOpNode(OpNode* node);

/**
 * @brief Prints in stdout the given OpQueue (used for debug purposes)
 * 
 * @param opQueue 
 */
void printOpQueue(OpQueue opQueue);

/**
 * @brief Frees memory from an OpNode struct
 * 
 * @param node 
 */
void freeOpNode(OpNode* node);

/**
 * @brief Frees memory from an OpQueue struct
 * 
 * @param opQueue 
 */
void freeOpQueue(OpQueue opQueue);
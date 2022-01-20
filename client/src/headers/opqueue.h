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
 * @brief Checks wether the queue is empty
 * 
 * @param queue 
 * @return int 
 */
int isEmpty(OpQueue queue);

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

/**
 * @brief Checks if node is valid
 * 
 * @param node 
 * @return int 
 */
int validateOpNode(OpNode* node);

/**
 * @brief Checks wether the queue contains incompatibilities between operations
 * 
 * @return int 
 */
int validateOpQueue(OpQueue);

/**
 * @brief checks if the operation type is in the queue
 * 
 * @param opQueue 
 * @param op 
 * @return int 
 */
int existsOp(OpQueue opQueue, int op);
#include "headers/opqueue.h"
#include "headers/consts.h"


OpNode* createOpNode(char op, char** subopts){
    OpNode* opNode;
    MALLOC(opNode, 1, OpNode);

    switch(op){
        case 'h':{
            printf("helper");
            break;
        }case 'f':{
            opNode->op = CONNECT;
            break;
        }case 'w':{
            opNode->op = WRITEDIR;
            break;
        }case 'W':{
            opNode->op = WRITEF;
            break;
        }case 'D':{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            opNode->op = CLIENTDIR;
            break;
        }case 'r':{ 
            opNode->op = READN;
            break;
        }case 'R':{ 
            opNode->op = READRAND;
            break;
        }case 'd':{
            opNode->op = DIRNAME;
            break;
        }case 't':{
            opNode->op = TIME;
            break;
        }case 'l':{ 
            opNode->op = LOCKN;
            break;
        }case 'u':{
            opNode->op = UNLOCKN;
            break;
        }case 'c':{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            opNode->op = REMOVEN;
            break;
        }case 'p':{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            opNode->op = PRINT;
            break;
        }

    }
    opNode->subopts = subopts;
    opNode->next = NULL;
    return opNode;
}

void pushOpNode(OpQueue* opQueue, OpNode* node){

    if(node == NULL){
        printf("The opNode is NULL");
        return;
    }
    if(*opQueue == NULL){
        *opQueue = node;
        return;
    }

    OpNode* curr = *opQueue;
    while(curr->next != NULL){
        curr = curr->next;
    }

    curr->next = node;
}


OpNode* popOpNode(OpQueue* opQueue){
    if(*opQueue == NULL){
        printf("Queue is empty\n");
        return NULL;
    }
    OpNode* deleted = *opQueue;
    if(*opQueue != NULL) *opQueue = (*opQueue)->next;
    
    return deleted;
}

void freeOpNode(OpNode* node){
    if(node == NULL){
        free(node);
        return;
    }
        
    if(node->subopts == NULL){
        free(node);
        return;
    }else{
        int i = 0;
        
        while(node->subopts[i] != NULL){
            free(node->subopts[i++]);
        }
            
        free(node->subopts);
        free(node);
    }
}

void freeOpQueue(OpQueue opQueue){
    OpNode* tmphead;
    while(opQueue!=NULL){
        tmphead = opQueue;
        opQueue = tmphead->next;
        freeOpNode(tmphead);
    }
}

void printOpNode(OpNode* node){
    if(node == NULL){
        printf("Node is NULL\n");
        return;
    }
    printf("Operation: %d\n",node->op);
    int i = 0;
    printf("Args: \n");
    if(node->subopts != NULL){
        while(node->subopts[i] != NULL){
        printf("  -%s\n",node->subopts[i]);
        i++;
    }
    }
    
}

void printOpQueue(OpQueue opQueue){
    if(opQueue == NULL){
        printf("Queue is empty\n");
        return;
    }
    printOpNode(opQueue);
    printf("\n");
    if(opQueue->next == NULL){
        return;
    }
    
    while(opQueue->next != NULL){
        printOpNode(opQueue->next);
        printf("\n");
        opQueue  = opQueue->next;
    }
}
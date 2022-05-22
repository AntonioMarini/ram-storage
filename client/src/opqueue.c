#include "headers/opqueue.h"
#include "headers/consts.h"
#include <ctype.h>
#include <string.h>

int wordsLen(char** words){
    if(words == NULL) return 0;
    int i = 0;

    if(!strcmp(words[i++]," ")) return 0;
    while(words[i] != NULL){
        i++;
    }
    
    return i;
}

int isNumber(char s[])
{
    for (int i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

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

int isEmpty(OpQueue queue){
    return queue == NULL;
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

int validateOpNode(OpNode* opNode){
    if(opNode == NULL) return 0;
    int len = wordsLen(opNode->subopts);
      
    switch(opNode->op){
        case CONNECT:{
            // deve contenere un solo argomento di tipo stringa
            if(len != 1)
                return 0;
            break;
        }case WRITEDIR:{ 
            //deve contenere primo parametro di tipo stringa e secondo parametro deve essere un n=*
            if(len < 1 || len > 2)
                return 0;
            break;
        }case WRITEF:{
            // deve contenere solo argomenti stringhe
            if(len < 1)
                return 0;
            break;
        }case CLIENTDIR:{ 
            // deve contenere una stringa
            if(len != 1)
                return 0;
            break;
        }case READN:{ 
            // deve contenere una lista di stringhe
            if(len < 1)
                return 0;
            break;
        }case READRAND:{ 
            // deve contenere una stringa e opzionalmente un intero n=*
            if(len < 1 || len > 2)
                return 0;
            break;
        }case DIRNAME:{
            // deve contenere una stringa
            if(len != 1)
                return 0;
            break;
        }case TIME:{
            // deve contenere un intero
            if(len != 1 && isNumber(opNode->subopts[0]))
                return 0;
            break;
        }case LOCKN:{ 
            // deve contenere una lista di stringhe 
            if(len < 1)
                return 0;
            break;
        }case UNLOCKN:{
            // deve contenere una lista di stringhe
            if(len < 1)
                return 0;
            break;
        }case REMOVEN:{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            // deve contenere una lista di stringhe
            if(len < 1)
                return 0;
            break;
        }case PRINT:{ // da usare congiuntamente a W (controllare se nella coda e gia presente)
            // non deve avere argomenti
            if(len  != 0)
                return 0;
            break;
        }
    }

    return 1;
}

int existsOp(OpQueue opQueue, int op){
    while(opQueue!= NULL){
        if(opQueue->op == op) return 1;
        opQueue = opQueue->next;
    }

    return 0;
}

int validateOpQueue(OpQueue opQueue){
    while(opQueue != NULL){
        if(!validateOpNode(opQueue)) return 0;
        opQueue = opQueue->next;
    }
    
    return 1;
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
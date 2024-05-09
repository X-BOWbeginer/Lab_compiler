#ifndef _HEADERNAME_H
#define _HEADERNAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct NODE {
    char* label;
    char* type;
    // int ,float,id,type,relop,t,nt,null
    char val[32];
    //char val[1];
    int numChildren;
    struct NODE** children;
    int line;
} Node;

Node* createNode(char* _label, char* _type, char* _val,int _line);
void addChild(Node* parent, Node* child);
void PrintNode(Node* Program,int depth);
int type_to_int(char* type);

#endif
#ifndef __INNERCODE_H__
#define __INNERCODE_H__
#include "semantic.h"
#include <assert.h>
#include <stdbool.h>
typedef struct Operand_ *Operand;
typedef struct InterCode_ *InterCode;
typedef struct InterCodes_ *InterCodes;
typedef struct Operand_
{
    enum
    {
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_TEMP,
        OP_TEMP_ADDR,
        OP_LABEL,
        OP_FUNCTION
    } kind;
    union
    {
        int var_no; //
        int value;  //
        char *name; // func const
    } u;
} Operand_;

typedef struct InterCode_
{
    enum
    {
        LABEL,
        FUNC,
        ASSIGN,
        ADD,
        SUB,
        MUL,
        DIV,
        GETADDR, // x:=&y
        GETVAL,  // x:=*y
        SETVAL,  // *x=y
        COPY,    // *x=*y
        GOTO,
        IF,
        RETURN,
        DEC,
        ARG,
        CALL,
        PARAM,
        READ,
        WRITE,
        TEST
    } kind;
    union
    {
        struct
        {
            Operand op;
        } singleOP; // LABEL FUNCTION GOTO RETURN ARG PARAM READ WRITE
        struct
        {
            Operand left;
            Operand right;
        } doubleOP; // ASSIGN GETVAL SETVAL CALL
        struct
        {
            Operand result;
            Operand op1;
            Operand op2;
        } tripleOP; // PLUS MINUS STAR DIV GETADDR
        struct
        {
            Operand op1;
            Operand op2;
            Operand label;
            char relop[32];
        } ifgotoOP; // IF
        struct
        {
            Operand op;
            int size;
        } decOP; // DEC
    } u;
} InterCode_;

typedef struct InterCodes_
{
    InterCode_ code;
    // bool isDelete;
    InterCodes prev, next;
} InterCodes_;

void setVariable();
void setFunction();
char *printop(Operand op);

void tranCond(Node *node, Operand label_true, Operand label_false);
void tranExp(Node *node, Operand place, Node *from);
void tranArgs(Node *node, Operand arg_list[], int index);

void tranProgram(Node *node);
void tranExtDefList(Node *node);
void tranExtDef(Node *node);
void tranFunDec(Node *node);
void tranCompSt(Node *node);
void tranVarDec(Node *node, int isFunc);
void tranDefList(Node *node);
void tranVarList(Node *node, int isFunc);
void tranParamDec(Node *node, int isFunc);
void tranStmtList(Node *node);
void tranStmt(Node *node);
void tranDef(Node *node);
void tranDecList(Node *node);
void tranDec(Node *node);

#endif

#include "node.h"
#include <assert.h>
#define HASH_SIZE 0x3fff
typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;

typedef struct Operand_ *Operand;

typedef struct Type_
{
    enum
    {
        BASIC,
        ARRAY,
        STRUCTURE,
        FUNCTION
    } kind;
    union
    {
        // 基本类型
        int basic;//int=0,float=1
        // 数组类型信息包括元素类型与数组大小构成
        struct
        {
            Type elem;
            int size;
        } array;
        // 结构体类型信息是一个链表
        FieldList structure;
        // 函数
        struct
        {
            FieldList params;
            Type returnType;
            int number_of_params;
            int line;
        } function;
    } u;
} Type_;

typedef struct FieldList_
{
    char *name;     // 域的名字
    Type type;      // 域的类型
    FieldList tail; // 下一个域
    FieldList next; // 冲突的话放这里
    int is_declare; // only for function
    Operand op;
    int size;

} FieldList_;
//
unsigned int hash_pjw(char *name);
void initHashtable(void);
void insert(FieldList f, int is_function);
FieldList find(char *name, int is_function);
//
FieldList VarDec(Node *root, Type basictype);
Type Specifier(Node *root);
void ExtDefList(Node *root);
void CompSt(Node *root, Type returnType);
void DefList(Node *root);
void Stmt(Node *root, Type returnType);
Type Exp(Node *root);
void program(Node *root); 
//
int TypeEqual(Type type1, Type type2);
void AllSymbol();

void printNode(Node *n);
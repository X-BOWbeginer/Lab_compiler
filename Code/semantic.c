#include "semantic.h"
#include <assert.h>

FieldList hashTable[HASH_SIZE];
FieldList funcTable[HASH_SIZE];
int lab3error = 0;

void AllSymbol()
{
    for (int i = 0; i < HASH_SIZE; i++)
        if (hashTable[i] != NULL)
            printf("name:%s,kind:%d\n", hashTable[i]->name, hashTable[i]->type->kind);
    for (int i = 0; i < HASH_SIZE; i++)
        if (funcTable[i] != NULL)
            printf("funcname:%s,kind:%d,is_declare%d\n", funcTable[i]->name, funcTable[i]->type->kind, funcTable[i]->is_declare);
}
void printNode(Node *n)
{
    if (n == NULL)
    {
        printf("null node\n");
        return;
    }
    printf("n->label=%s,n->line=%d,n->numChildren=%d,n->type=%s,n->val=%s\n", n->label, n->line, n->numChildren, n->type, n->val);
    PrintNode(n, 0);
}
void Dec_bt_undef()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (funcTable[i] != NULL)
        {
            FieldList f = funcTable[i];
            if (f->is_declare == 1)
            {
                printf("Error type 18 at line %d: Undefined function \"%s\".\n", f->type->u.function.line, f->name);
            }
        }
    }
}

unsigned int hash_pjw(char *name)
{
    unsigned int val = 0;
    unsigned int i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~HASH_SIZE)
            val = (val ^ (i >> 12)) & HASH_SIZE;
        ;
    }
    return val % HASH_SIZE;
    ;
}

void initHashtable(void)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        hashTable[i] = NULL;
        funcTable[i] = NULL;
    }
    // 插入'int read()'函数
    {
        FieldList field1 = (FieldList)malloc(sizeof(FieldList_));
        field1->name = "read";
        Type typ1 = (Type)malloc(sizeof(Type_));
        typ1->kind = FUNCTION;
        Type typ1_return = (Type)malloc(sizeof(Type_));
        typ1_return->kind = BASIC;
        typ1_return->u.basic = 0;
        typ1->u.function.returnType = typ1_return;
        typ1->u.function.number_of_params = 0;
        typ1->u.function.params = NULL;
        field1->type = typ1;
        insert(field1, 1);
    }
    // 插入 'int write(int write_param_name)'函数
    {

        FieldList field2 = (FieldList)malloc(sizeof(FieldList_));
        field2->name = "write";
        Type typ2 = (Type)malloc(sizeof(Type_));
        typ2->kind = FUNCTION;
        Type typ2_return = (Type)malloc(sizeof(Type_));
        typ2_return->kind = BASIC;
        typ2_return->u.basic = 0;
        typ2->u.function.returnType = typ2_return;
        typ2->u.function.number_of_params = 1;
        FieldList write_param = (FieldList)malloc(sizeof(FieldList_));
        write_param->name = "write_param_name_is_n";
        Type write_param_typ = (Type)malloc(sizeof(Type_));
        write_param_typ->kind = BASIC;
        write_param_typ->u.basic = 0;
        write_param->type = write_param_typ;
        typ2->u.function.params = write_param;
        field2->type = typ2;
        insert(field2, 1);
    }
}

void insert(FieldList f, int is_function)
{
    assert(f != NULL);
    // assert(f->name != NULL);
    // printf("dsadas\n");
    // printf("%s\n", f->name);
    if (f->name == NULL)
    {
        return;
    }
    // f->is_defined = 1;
    unsigned int key = hash_pjw(f->name);

    FieldList *table; // 哈希表对应的指针
    if (is_function == 0)
    {
        table = hashTable;
    }
    else
    {
        table = funcTable;
    }

    FieldList p = table[key];
    if (p == NULL)
    { // 哈希桶为空，直接加到第一个位置
        table[key] = f;
        return;
    }

    // 在哈希桶中搜索是否已经有了该元素
    while (p->next != NULL)
    {
        if (strcmp(p->name, f->name) == 0)
        { // 已经有了，不做处理
            return;
        }
        p = p->next;
    }

    // 到达链表末尾，添加元素
    if (strcmp(p->name, f->name) == 0)
    { // 已经有了，不做处理
        return;
    }
    p->next = f;
}

FieldList find(char *name, int is_function)
{
    // assert(name != NULL);
    // printf("%s\n", name);
    if (name == NULL)
    {
        return NULL;
    }
    unsigned int key = hash_pjw(name);
    FieldList p = NULL;
    if (is_function == 0)
    {
        p = hashTable[key];
    }
    else
    {
        p = funcTable[key];
    }
    while (p != NULL)
    {
        if (strcmp(p->name, name) == 0)
        {
            return p;
        }
        else
        {
            p = p->next;
        }
    }
    return NULL;
}

void program(Node *root) // ExtDefList
{
    assert(root->numChildren == 1);
    ExtDefList(root->children[0]);
    Dec_bt_undef();
}
void ExtDefList(Node *root) // ExtDef ExtDefList
{
    // return;
    // printf("inExtDefList\n");
    Node *ExtDefList = root;
    if (strcmp(ExtDefList->type, "null") == 0)
        return;
    while (ExtDefList->numChildren != 0)
    {
        // if (strcmp(ExtDefList->type, "null") == 0)
        // return;
        // printf("%s\n",ExtDefList->type);
        //  printf("%s\n", ExtDefList->children[0]->label);
        // printNode(ExtDefList);
        //  assert(ExtDefList->numChildren == 2);
        Node *ExtDef = ExtDefList->children[0];
        // printf("basic\n");
        // printNode(ExtDef->children[0]);
        Type basictype = Specifier(ExtDef->children[0]);
        // printf("%s\n", ExtDef->children[1]->label);
        if (strcmp(ExtDef->children[1]->label, "ExtDecList") == 0)
        { // Specifier ExtDecList SEMI
            Node *varList = ExtDef->children[1];
            for (int i = 0; i < varList->numChildren; i += 2)
            {
                FieldList field = VarDec(varList->children[i], basictype);
                if (find(field->name, 0) != NULL)
                    printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", ExtDef->line, field->name);
                else
                    insert(field, 0);
            }
        }
        else if (strcmp(ExtDef->children[1]->label, "SEMI") == 0)
        { // Specifier SEMI
        }
        else if (strcmp(ExtDef->children[2]->label, "SEMI") == 0) // Specifier FunDec SEMI 2.1
        {
            // printf("Specifier FunDec SEMI 2.1\n");
            FieldList field = (FieldList)malloc(sizeof(FieldList_));
            Node *fundec = ExtDef->children[1];
            field->name = ExtDef->children[1]->children[0]->val;
            Type funtyp = (Type)malloc(sizeof(Type_));
            funtyp->kind = FUNCTION;
            funtyp->u.function.returnType = basictype;
            // main function
            funtyp->u.function.params = NULL;
            funtyp->u.function.number_of_params = 0;
            funtyp->u.function.line = fundec->line;
            // printf("spety\n");
            // printNode(fundec);
            if (fundec->numChildren == 4) // ID LP VarList RP
            {
                // printf("spety\n");
                Node *VarList = ExtDef->children[1]->children[2];
                // printNode(VarList);
                while (VarList->numChildren == 3) // ParamDec COMMA VarList
                {
                    // printf("spety\n");
                    // printNode(VarList->children[0]->children[0]);
                    Type spetyp = Specifier(VarList->children[0]->children[0]);
                    FieldList varfield = VarDec(VarList->children[0]->children[1], spetyp);

                    VarList = VarList->children[2];
                    funtyp->u.function.number_of_params++;
                    varfield->tail = funtyp->u.function.params; // i think null is ok
                    funtyp->u.function.params = varfield;
                } // ParamDec
                  // printf("spety2\n");

                Type spetyp = Specifier(VarList->children[0]->children[0]);
                FieldList varfield = VarDec(VarList->children[0]->children[1], spetyp);

                funtyp->u.function.number_of_params++;
                varfield->tail = funtyp->u.function.params; // i think null is ok
                funtyp->u.function.params = varfield;
            }
            field->type = funtyp;
            if (find(field->name, 1) != NULL)
            {
                FieldList exist_field = find(field->name, 1);
                if (TypeEqual(field->type, exist_field->type) != 0)
                {
                    if (TypeEqual(field->type->u.function.returnType, exist_field->type->u.function.returnType) != 0)
                        // para type TODO:
                        return;
                }
                printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", fundec->line, field->name);
            }
            else
            {
                field->is_declare = 1;
                // printf("DECLARE!!!field->name=%s,field->is_defined=%d\n", field->name, field->is_declare);
                insert(field, 1);
            }
        }
        else
        { // Specifier FunDec CompSt
            // printf("Specifier FunDec CompSt\n");
            FieldList field = (FieldList)malloc(sizeof(FieldList_));
            Node *fundec = ExtDef->children[1];
            field->name = ExtDef->children[1]->children[0]->val;
            Type funtyp = (Type)malloc(sizeof(Type_));
            funtyp->kind = FUNCTION;
            funtyp->u.function.returnType = basictype;
            // main function
            funtyp->u.function.params = NULL;
            funtyp->u.function.number_of_params = 0;
            // printf("spety\n");
            // printNode(fundec);
            if (fundec->numChildren == 4) // ID LP VarList RP
            {
                // printf("spety\n");
                Node *VarList = ExtDef->children[1]->children[2];
                // printNode(VarList);
                while (VarList->numChildren == 3) // ParamDec COMMA VarList
                {
                    // printf("spety\n");
                    // printNode(VarList->children[0]->children[0]);
                    Type spetyp = Specifier(VarList->children[0]->children[0]);
                    FieldList varfield = VarDec(VarList->children[0]->children[1], spetyp);
                    if (find(varfield->name, 0) != NULL)
                    {
                        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", ExtDef->line, varfield->name);
                    }
                    else
                    {

                        insert(varfield, 0);
                    }
                    VarList = VarList->children[2];
                    funtyp->u.function.number_of_params++;
                    varfield->tail = funtyp->u.function.params; // i think null is ok
                    funtyp->u.function.params = varfield;
                } // ParamDec
                  // printf("spety2\n");

                Type spetyp = Specifier(VarList->children[0]->children[0]);
                FieldList varfield = VarDec(VarList->children[0]->children[1], spetyp);
                if (find(varfield->name, 0) != NULL)
                {
                    printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", ExtDef->line, varfield->name);
                }
                else
                {
                    insert(varfield, 0);
                }
                funtyp->u.function.number_of_params++;
                varfield->tail = funtyp->u.function.params; // i think null is ok
                funtyp->u.function.params = varfield;
            }
            field->type = funtyp;
            if (find(field->name, 1) != NULL)
            {
                FieldList f = find(field->name, 1);
                if (f->is_declare != 1)
                    printf("Error type 4 at Line %d: Redefined function \"%s\".\n", ExtDef->line, field->name);
                else
                {
                    unsigned int key = hash_pjw(field->name);
                    FieldList p = funcTable[key];
                    while (strcmp(p->name, field->name) != 0)
                    {
                        p = p->next;
                    }
                    p->is_declare = 0;
                    FieldList exist_field = find(field->name, 1);
                    if (TypeEqual(field->type, exist_field->type) != 0)
                    {
                        if (TypeEqual(field->type->u.function.returnType, exist_field->type->u.function.returnType) != 0)
                            // para type TODO:
                            return;
                    }
                    printf("Error type 19 at line %d: Inconsistent declaration of function '%s'\n", fundec->line, fundec->label);
                }
            }
            else
            {
                // printf("DEFINE!!!!field->name=%s\n", field->name);
                field->is_declare = 0;
                insert(field, 1);
            }
            // CompSt->LC DefList StmtList RC
            // printNode(ExtDef->children[2]);
            CompSt(ExtDef->children[2], basictype);
        }
        if (ExtDefList->children[1] == NULL) // ExtDef
        {

            return;
        }
        ExtDefList = ExtDefList->children[1];
        // printf("qweqeqweqe\n");
    }
}

Type Specifier(Node *root)
{
    // return NULL;
    // printf("inSpecifier\n");
    Node *specifier = root;
    Type spe = (Type)malloc(sizeof(Type_));
    // printNode(root);
    //   printf("numchi%d,line%d,\n",Specifier->numChildren,Specifier->line);
    if (strcmp(specifier->children[0]->label, "TYPE") == 0) // TYPE
    {
        // printf("type\n");
        spe->kind = BASIC;
        // printNode(Specifier->children[0]);
        if (strcmp(specifier->children[0]->val, "int") == 0) // int
        {
            // printf("intint\n");
            spe->u.basic = 0;
        }
        else
        {
            // printf("dsa\n");
            spe->u.basic = 1;
        }
    }
    else // StructSpecifier
    {
        lab3error = 1;
        // 设置符号表项为结构体类型
        spe->kind = STRUCTURE;

        // 如果有结构体标签
        if (root->children[0]->numChildren == 2)
        {
            char *structName = root->children[0]->children[1]->children[0]->val;
            // 在符号表中查找结构体
            FieldList field = find(structName, 0);
            // 如果未找到该结构体
            if (field == NULL)
            {
                printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", root->line, structName);
                spe->u.structure = NULL;
            }
            else if (field->type != NULL)
            { // 如果已经找到该结构体
                // 释放当前符号表项并返回找到的结构体类型
                free(spe);
                return field->type;
            }
            // 如果未找到该结构体，设置当前符号表项为 NULL
            spe->u.structure = NULL;
        }

        else
        { // STRUCT OptTag LC DefList RC
            Node *defList = root->children[0]->children[3];
            spe->u.structure = NULL;
            while (strcmp(defList->type, "null") != 0)
            { // Def DefList
                Node *Def = defList->children[0];
                Type basictype = Specifier(Def->children[0]);
                Node *DecList = Def->children[1];
                // if(Def==NULL)
                // break;
                // printNode(DecList);
                while (DecList->numChildren == 3)
                { // Dec COMMA DecList
                    FieldList field = VarDec(DecList->children[0]->children[0], basictype);
                    if (DecList->children[0]->numChildren != 1)
                        printf("Error type 15 at Line %d: Variable %s in struct is initialized.\n", Def->line, field->name);
                    FieldList temp = spe->u.structure;
                    while (temp != NULL)
                    {
                        if (strcmp(temp->name, field->name) == 0)
                        {
                            printf("Error type 15 at Line %d: Redefined field \"%s\".\n", Def->line, field->name);
                            break;
                        }
                        temp = temp->tail;
                    }
                    if (temp == NULL)
                    {
                        if (find(field->name, 0) != NULL)
                            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", Def->line, field->name);
                        else
                        {
                            insert(field, 0);
                            field->tail = spe->u.structure;
                            spe->u.structure = field;
                        }
                    }
                    DecList = DecList->children[2];
                }
                FieldList field = VarDec(DecList->children[0]->children[0], basictype);
                if (DecList->children[0]->numChildren != 1)
                    printf("Error type 15 at Line %d: Variable \"%s\" in struct is initialized.\n", Def->line, field->name);
                FieldList temp = spe->u.structure;
                while (temp != NULL)
                {
                    if (strcmp(temp->name, field->name) == 0)
                    {
                        printf("Error type 15 at Line %d: Redefined field \"%s\".\n", Def->line, field->name);
                        break;
                    }
                    temp = temp->tail;
                }
                if (temp == NULL)
                {
                    if (find(field->name, 0) != NULL)
                        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", Def->line, field->name);
                    else
                    {
                        insert(field, 0);
                        field->tail = spe->u.structure;
                        spe->u.structure = field;
                    }
                }
                defList = defList->children[1];
            }
            if (root->children[0]->children[1] != NULL)
            { // OptTag exist
                FieldList field = (FieldList)malloc(sizeof(FieldList_));
                field->type = spe;
                char *s = root->children[0]->children[1]->children[0]->val;
                field->name = s;
                if (find(field->name, 0) != NULL)
                    printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", root->line, field->name);
                else
                    insert(field, 0);
            }
            return spe;
        }
    }
    return spe;
}

FieldList VarDec(Node *root, Type basictype)
{
    // printf("inVarDec\n");
    Node *vardec = root;
    FieldList field;
    // printf("kind%d\n", basictype->kind);
    if (strcmp(vardec->children[0]->label, "ID") == 0) // ID
    {
        // printf("kind%d\n", basictype->kind);
        field = (FieldList)malloc(sizeof(FieldList_));
        field->type = basictype;
        field->name = vardec->children[0]->val;
        return field;
    }
    else // VarDec LB INT RB
    {
        /* code */
        // FieldList field = (FieldList)malloc(sizeof(FieldList_));
        field = VarDec(vardec->children[0], basictype);
        if (field == NULL)
            return NULL;
        Type temp = field->type; // INT
        Type t = malloc(sizeof(struct Type_));
        t->kind = 1; // array
        t->u.array.size = atoi(root->children[2]->val);
        t->u.array.elem = basictype;
        if (temp->kind != 1)
        {
            field->type = t;
            return field;
        }
        while (temp->u.array.elem->kind == 1) // find the end of this array
        {
            temp = temp->u.array.elem;
        }
        temp->u.array.elem = t;
        // printtype(f->type);
        return field;
    }
}
void CompSt(Node *root, Type returnType) // LC DefList StmtList RC
{
    // return;
    // printf("inCompSt\n");
    // printNode(root);
    Node *CompSt = root;
    // printf("aa\n");
    // printNode(CompSt->children[1]);
    DefList(CompSt->children[1]);
    // printf("bb\n");
    Node *StmtList = CompSt->children[2];

    if (strcmp(StmtList->type, "null") == 0)
        return;
    while (StmtList != NULL)
    {
        // printf("CompSt\n");
        // printNode(StmtList);
        Node *stmt = StmtList->children[0];

        if (stmt == NULL)
            return;

        Stmt(stmt, returnType);
        StmtList = StmtList->children[1];
    }
}
void DefList(Node *root)
{
    Node *DefList = root;
    if (strcmp(DefList->type, "null") == 0)
    {
        return;
    }
    while (strcmp(DefList->type, "null") != 0)
    {
        Node *Def = DefList->children[0];
        Type deftyp = Specifier(Def->children[0]);
        Node *DecList = Def->children[1];
        while (DecList->numChildren == 3)
        {
            FieldList varfield = VarDec(DecList->children[0]->children[0], deftyp);
            if (find(varfield->name, 0) != NULL)
            {
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", DecList->line, varfield->name);
            }
            else
            {
                insert(varfield, 0);
            }
            DecList = DecList->children[2];
        }
        FieldList varfield = VarDec(DecList->children[0]->children[0], deftyp);
        if (find(varfield->name, 0) != NULL)
        {
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", DecList->line, varfield->name);
        }
        else
        {
            insert(varfield, 0);
        }
        DefList = DefList->children[1];
    }
}
void Stmt(Node *root, Type returnType)
{
    Node *stmt = root;
    if (strcmp(stmt->children[0]->label, "Exp") == 0)
    {
        Exp(stmt->children[0]);
    }
    else if (stmt->numChildren == 1)
    {
        CompSt(stmt->children[0], returnType);
    }
    else if (strcmp(stmt->children[0]->label, "RETURN") == 0)
    {
        Type exptyp = Exp(stmt->children[1]);
        if (exptyp == NULL)
        {
            return;
        }
        if (TypeEqual(exptyp, returnType) == 0)
        {
            printf("Error type 8 at Line %d: Type mismatched for return.\n", stmt->line);
        }
    }
    else if (stmt->numChildren == 7)
    {
        Type exptyp = Exp(stmt->children[2]);
        if (exptyp == NULL)
        {
            return;
        }
        if (!((exptyp->kind == BASIC) && (exptyp->u.basic == 0)))
        {
            printf("Error type 5 at Line %d: Only type INT could be used for judgement.\n", stmt->line);
        }
        Stmt(stmt->children[4], returnType);
        Stmt(stmt->children[6], returnType);
    }
    else if (stmt->numChildren == 5)
    {
        Type exptyp = Exp(stmt->children[2]);
        if (exptyp == NULL)
        {
            return;
        }
        if (!((exptyp->kind == BASIC) && (exptyp->u.basic == 0)))
        {
            printf("Error type 5 at Line %d: Only type INT could be used for judgement.\n", stmt->line);
        }
        Stmt(stmt->children[4], returnType);
    }
}

Type Exp(Node *root)
{
    // return NULL;
    // printf("inExpp\n");
    Node *exp = root;
    if (exp == NULL)
    {
        return NULL;
    }
    else if (strcmp(exp->children[0]->label, "ID") == 0 && exp->numChildren == 1)
    {
        // ID
        FieldList field = find(exp->children[0]->val, 0);
        if (field != NULL)
        {
            return field->type;
        }
        printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", exp->line, exp->children[0]->val);
        return NULL;
    }
    else if (strcmp(exp->children[0]->label, "INT") == 0)
    {
        // INT
        return &(Type_){.kind = BASIC, .u.basic = 0};
    }
    else if (strcmp(exp->children[0]->label, "FLOAT") == 0)
    {
        // FLOAT
        return &(Type_){.kind = BASIC, .u.basic = 1};
    }

    // not
    else if (strcmp(exp->children[0]->label, "LP") == 0 || strcmp(exp->children[0]->label, "MINUS") == 0 || strcmp(exp->children[0]->label, "NOT") == 0)
    {
        // LP Exp RP, MINUS Exp, NOT Exp
        return Exp(exp->children[1]);
    }
    else if (strcmp(exp->children[1]->label, "PLUS") == 0 || strcmp(exp->children[1]->label, "MINUS") == 0 ||
             strcmp(exp->children[1]->label, "STAR") == 0 || strcmp(exp->children[1]->label, "DIV") == 0)
    {
        // add
        Type leftType = Exp(exp->children[0]);
        Type rightType = Exp(exp->children[2]);
        if (TypeEqual(leftType, rightType) == 0)
        {
            if (leftType != NULL && rightType != NULL)
            {
                printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->line);
            }
            return NULL;
        }
        return leftType;
    }
    else if (strcmp(exp->children[1]->label, "AND") == 0 || strcmp(exp->children[1]->label, "OR") == 0 || strcmp(exp->children[1]->label, "RELOP") == 0)
    {
        // and, or, relop
        Type leftType = Exp(exp->children[0]);
        Type rightType = Exp(exp->children[2]);
        if (TypeEqual(leftType, rightType) == 0)
        {
            if (leftType != NULL && rightType != NULL)
            {
                printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->line);
            }
            return NULL;
        }
        return &(Type_){.kind = BASIC, .u.basic = 0};
    }

    // exp * exp
    else if (strcmp(root->children[1]->label, "ASSIGNOP") == 0)
    {
        int numChildren = root->children[0]->numChildren;
        int isVariable = 0;

        if (numChildren == 1)
        {
            isVariable = strcmp(root->children[0]->children[0]->label, "ID") == 0;
        }
        else if (numChildren == 3)
        {
            isVariable = strcmp(root->children[0]->children[0]->label, "Exp") == 0 &&
                         strcmp(root->children[0]->children[1]->label, "DOT") == 0 &&
                         strcmp(root->children[0]->children[2]->label, "ID") == 0;
        }
        else if (numChildren == 4)
        {
            isVariable = strcmp(root->children[0]->children[0]->label, "Exp") == 0 &&
                         strcmp(root->children[0]->children[1]->label, "LB") == 0 &&
                         strcmp(root->children[0]->children[2]->label, "Exp") == 0 &&
                         strcmp(root->children[0]->children[3]->label, "RB") == 0;
        }

        if (!isVariable)
        {
            printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", root->line);
            return NULL;
        }

        Type typ1 = Exp(root->children[0]);
        Type typ2 = Exp(root->children[2]);

        if (TypeEqual(typ1, typ2) == 0)
        {
            if (typ1 != NULL && typ2 != NULL)
            {
                printf("Error type 5 at Line %d: Type mismatched for assignment.\n", root->line);
            }
            return NULL;
        }
        return typ1;
    }

    // ID LP RP
    else if (strcmp(exp->children[0]->label, "ID") == 0 && exp->numChildren == 3)
    {
        FieldList fie = find(exp->children[0]->val, 1);

        if (fie == NULL)
        {
            FieldList fie2 = find(exp->children[0]->val, 0);

            if (fie2 != NULL)
            {
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->line, exp->children[0]->val);
            }
            else
            {
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n", exp->line, exp->children[0]->val);
            }
            return NULL;
        }

        Type definedType = fie->type;
        Type typ = calloc(1, sizeof(Type_));
        typ->kind = FUNCTION;
        // printf("definedType =%s\n", definedType->u.function.params->name);
        if (TypeEqual(typ, definedType) == 0)
        {
            printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments.\n", exp->line, exp->children[0]->val);
            return NULL;
        }
        else
        {
            return definedType->u.function.returnType;
        }
    }

    // ID LP Args RP
    else if ((strcmp(exp->children[0]->label, "ID") == 0) && exp->numChildren == 4)
    {
        FieldList fie = find(exp->children[0]->val, 1);
        if (fie == NULL)
        {
            FieldList fie2 = find(exp->children[0]->val, 0);
            if (fie2 != NULL)
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->line, exp->children[0]->val);
            else
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n", exp->line, exp->children[0]->val);
            return NULL;
        }

        Type definedType = fie->type;
        Type typ = (Type)malloc(sizeof(Type_));
        typ->kind = FUNCTION;
        typ->u.function.number_of_params = 0;
        typ->u.function.params = NULL;

        for (Node *temp = exp->children[2]; temp->numChildren != 1; temp = temp->children[2])
        {
            Type tempType = Exp(temp->children[0]);
            FieldList tempField = &(FieldList_){.name = "asuka", .type = tempType, .tail = typ->u.function.params};
            typ->u.function.number_of_params++;
            typ->u.function.params = tempField;
        }

        Type tempType = Exp(exp->children[2]->children[0]);
        FieldList tempField = &(FieldList_){.name = "asuka", .type = tempType, .tail = typ->u.function.params};
        typ->u.function.number_of_params++;
        typ->u.function.params = tempField;
        // printf("typ=%d\n", typ->u.function.params->type->kind);

        // printf("definedType=%d\n",definedType->u.function.params->type->kind);
        if (!TypeEqual(typ, definedType))
        {
            if (strcmp(fie->name, "write") != 0)
            {
                printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments.\n", exp->line, exp->children[0]->val);
                return NULL;
            }
        }
        else
            return definedType->u.function.returnType;
    }

    // Exp DOT ID
    else if (strcmp(exp->children[1]->label, "DOT") == 0)
    {
        Type typ1 = Exp(exp->children[0]);
        if (typ1->kind != STRUCTURE)
        {
            Node *temp = exp->children[0];
            char *s = "error";
            if (temp->numChildren == 1 && strcmp(temp->children[0]->label, "ID") == 0)
                s = temp->children[0]->val;
            else if (temp->numChildren == 3 && strcmp(temp->children[2]->label, "ID") == 0)
                s = temp->children[0]->val;
            else if (temp->numChildren == 4 && strcmp(temp->children[0]->label, "Exp") == 0 && strcmp(temp->children[0]->children[0]->label, "ID") == 0)
                s = temp->children[0]->children[0]->val;

            if (find(s, 0) != NULL)
                printf("Error type 13 at Line %d: Illegal use of \".\".\n", exp->line);

            return NULL;
        }

        char *s = exp->children[2]->val;
        FieldList temp = typ1->u.structure;
        while (temp != NULL)
        {
            if (strcmp(temp->name, s) == 0)
                return temp->type;

            temp = temp->tail;
        }

        printf("Error type 14 at Line %d: Non-existent field \"%s\".\n", exp->line, s);
        return NULL;
    }

    // Exp LB Exp RB
    else if (strcmp(exp->children[1]->label, "LB") == 0)
    {
        Type typ1 = Exp(exp->children[0]);
        if (typ1->kind != ARRAY)
        {
            Node *temp = exp->children[0];
            char *s = "error";
            if (temp->numChildren == 1 && strcmp(temp->children[0]->label, "ID") == 0)
                s = temp->children[0]->val;
            else if (temp->numChildren == 3 && strcmp(temp->children[2]->label, "ID") == 0)
                s = temp->children[0]->val;
            else if (temp->numChildren == 4 && strcmp(temp->children[0]->label, "Exp") == 0 && strcmp(temp->children[0]->children[0]->label, "ID") == 0)
                s = temp->children[0]->children[0]->val;

            if (find(s, 0) != NULL)
                printf("Error type 10 at Line %d: \"%s\" is not an array.\n", exp->line, s);

            return NULL;
        }

        Type temp = Exp(exp->children[2]);
        if (temp->kind != BASIC || temp->u.basic == 1)
        {
            printf("Error type 12 at Line %d: \"%s\" is not an integer.\n", exp->line, exp->children[2]->val);
            return NULL;
        }

        return typ1->u.array.elem;
    }

    else
    {
        // printf("unexpect\n");
        return NULL;
    }
}

int TypeEqual(Type type1, Type type2)
{
    if ((type1 == NULL) || (type2 == NULL))
        return 0;
    // printf("inTypeEqual\n");
    //  return 1;
    if (type1->kind != type2->kind)
        return 0;
    else
        switch (type1->kind)
        {
        case BASIC:
        {
            // printf("BASIC\n");
            if (type1->u.basic == type2->u.basic)
                return 1;
            else
                return 0;
        }
        break;
        case ARRAY:
        {
            // printf("ARRAY\n");
            if (TypeEqual(type1->u.array.elem, type2->u.array.elem) == 1)
                return 1;
            else
                return 0;
        }
        break;
        case STRUCTURE:
        {
            // printf("STRUCT\n");
            if (TypeEqual(type1->u.structure->type, type2->u.structure->type) == 1 && strcmp(type1->u.structure->name, type2->u.structure->name) == 0)
                return 1;
            else
                return 0;
        }
        break;
        case FUNCTION:
        {
            // printf("FUNCTION\n");
            if (type1->u.function.number_of_params != type2->u.function.number_of_params)
                return 0;
            FieldList param1 = type1->u.function.params;
            FieldList param2 = type2->u.function.params;
            for (int i = 0; i < type1->u.function.number_of_params; i++)
            {
                if (TypeEqual(param1->type, param2->type) == 0)
                    return 0;
                // printf("1=%s\n", param1->name);
                // printf("2=%s\n", param2->name);
                param1 = param1->tail;
                param2 = param2->tail;
            }
            return 1;
        }
        break;
        default:
        {
            return 0;
        }
        break;
        }
}
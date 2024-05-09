#include "inter.h"

InterCodes head;
InterCodes tail;
extern FieldList hashTable[HASH_SIZE];
extern FieldList funcTable[HASH_SIZE];
int varCnt = 1;
int tmpCnt = 1;
int labelCnt = 1;
Operand zero;
void t(char *i)
{
    printf("%s\n", i);
}
void printlist(InterCodes s)
{
    printf("HELLO\n");

    while (s)
    {
        printf("s->code.kind=%d\n", s->code.kind);
        s = s->next;
    }
}

void initInterCodes()
{
    head = (InterCodes)malloc(sizeof(InterCodes_));
    head->prev = head->next = NULL;
    tail = head;
    head->code.kind = TEST;
    zero = (Operand)malloc(sizeof(Operand_));
    zero->kind = OP_CONSTANT;
    zero->u.value = 0;
    // printlist(head);
    setVariable();
    setFunction();
}

int returnSize(Type type)
{
    if (type->kind == ARRAY)
    {
        return type->u.array.size * returnSize(type->u.array.elem);
    }
    else if (type->kind == BASIC)
        return 4;
}
void setVariable()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (hashTable[i] == NULL)
            continue;
        else
        {
            FieldList p = hashTable[i];
            while (p != NULL)
            {
                Operand op = (Operand)malloc(sizeof(Operand_));
                if (p->type->kind == ARRAY)
                {
                    op->kind = OP_ADDRESS;
                    p->size = returnSize(p->type);
                }
                else
                {
                    op->kind = OP_VARIABLE;
                }
                op->u.var_no = varCnt;
                varCnt++;
                p->op = op;
                // printf("op->kind=%d, op->u.name=%d\n", op->kind, op->u.var_no);
                p = p->next;
            }
        }
    }
}
void setFunction()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (funcTable[i] == NULL)
            continue;
        else
        {
            FieldList p = funcTable[i];
            while (p != NULL)
            {
                Operand op = (Operand)malloc(sizeof(Operand_));
                op->kind = OP_FUNCTION;
                op->u.name = p->name;
                // varCnt++;
                p->op = op;
                // printf("op->kind=%d, op->u.name=%s\n", op->kind, op->u.name);
                p = p->next;
            }
        }
    }
}
Operand newConst(int val)
{
    Operand op = (Operand)malloc(sizeof(Operand_));
    op->kind = OP_CONSTANT;
    op->u.value = val;
    return op;
}

Operand newTemp(int addr)
{
    Operand op = (Operand)malloc(sizeof(Operand_));
    // op->kind = OP_TEMP;
    if (addr)
        op->kind = OP_TEMP_ADDR;
    else
        op->kind = OP_TEMP;
    op->u.var_no = tmpCnt;
    tmpCnt++;
    return op;
}

Operand newLabel()
{
    Operand op = (Operand)malloc(sizeof(Operand_));
    op->kind = OP_LABEL;
    op->u.var_no = labelCnt;
    labelCnt++;
    return op;
}
//
void singleOP(int kind, Operand op)
{

    InterCodes p = (InterCodes)malloc(sizeof(InterCodes_));
    p->code.kind = kind;
    p->code.u.singleOP.op = op;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    // printf("single\n");
    tail = tail->next;
    // printlist(head);
}

void doubleOP(int kind, Operand left, Operand right)
{
    InterCodes p = (InterCodes)malloc(sizeof(InterCodes_));
    p->code.kind = kind;
    p->code.u.doubleOP.left = left;
    p->code.u.doubleOP.right = right;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}
void tripleOP(int kind, Operand result, Operand op1, Operand op2)
{
    InterCodes p = (InterCodes)malloc(sizeof(InterCodes_));
    p->code.kind = kind;
    p->code.u.tripleOP.result = result;
    p->code.u.tripleOP.op1 = op1;
    p->code.u.tripleOP.op2 = op2;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}
void ifgotoOP(Operand label, Operand op1, Operand op2, char relop[32])
{
    // printf("if\n");
    InterCodes p = (InterCodes)malloc(sizeof(InterCodes_));
    p->code.kind = IF;
    p->code.u.ifgotoOP.label = label;
    p->code.u.ifgotoOP.op1 = op1;
    p->code.u.ifgotoOP.op2 = op2;
    strcpy(p->code.u.ifgotoOP.relop, relop);

    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}
void decOP(Operand op, int size)
{
    InterCodes p = (InterCodes)malloc(sizeof(InterCodes_));
    p->code.kind = DEC;
    p->code.u.decOP.op = op;
    p->code.u.decOP.size = size;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

char *printop(Operand op)
{
    char *res = (char *)malloc(40);
    // printf("op->kind=%d\n", op->kind);
    switch (op->kind)
    {
    case OP_VARIABLE:
        sprintf(res, "v%d", op->u.var_no);
        break;
    case OP_TEMP:
        sprintf(res, "t%d", op->u.var_no);
        break;
    case OP_CONSTANT:
        sprintf(res, "#%d", op->u.value);
        break;
    case OP_LABEL:
        sprintf(res, "label%d", op->u.var_no);
        break;
    case OP_FUNCTION:
        sprintf(res, "%s", op->u.name);
        break;
    case OP_ADDRESS:
        sprintf(res, "*v%d", op->u.var_no);
        break;
    case OP_TEMP_ADDR:
        sprintf(res, "*t%d", op->u.var_no);
        break;
    default:
        sprintf(res, "unexpected!!");
        break;
    }
    return res;
}
void writeInterCodes(const char *fielname)
{
    // printlist(head);
    //  printf("head->code.kind=%d\n", head->code.kind);
    InterCodes p = head->next;
    // printf("p->code.kind=%d\n", p->code.kind);
    // assert(p!= NULL);
    FILE *f = fopen(fielname, "w");
    // printf("hello\n");
    while (p)
    {
        assert(p != NULL);
        // printf("p->code.kind=%d", p->code.kind);
        switch (p->code.kind)

        {
        case LABEL:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "LABEL %s :\n", op);
            break;
        }
        case FUNC:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "FUNCTION %s :\n", op);
            break;
        }
        case ASSIGN:
        {
            // fprintf(f, "assign\n");
            char *l = printop(p->code.u.doubleOP.left);
            char *r = printop(p->code.u.doubleOP.right);
            fprintf(f, "%s := %s\n", l, r);
            break;
        }
        case ADD:
        {
            char *r = printop(p->code.u.tripleOP.result);
            char *op1 = printop(p->code.u.tripleOP.op1);
            char *op2 = printop(p->code.u.tripleOP.op2);
            fprintf(f, "%s := %s + %s\n", r, op1, op2);
        }
        break;
        case SUB:
        {
            char *r = printop(p->code.u.tripleOP.result);
            char *op1 = printop(p->code.u.tripleOP.op1);
            char *op2 = printop(p->code.u.tripleOP.op2);
            fprintf(f, "%s := %s - %s\n", r, op1, op2);
        }
        break;
        case MUL:
        {
            char *r = printop(p->code.u.tripleOP.result);
            char *op1 = printop(p->code.u.tripleOP.op1);
            char *op2 = printop(p->code.u.tripleOP.op2);
            fprintf(f, "%s := %s * %s\n", r, op1, op2);
        }
        break;
        case DIV:
        {
            char *r = printop(p->code.u.tripleOP.result);
            char *op1 = printop(p->code.u.tripleOP.op1);
            char *op2 = printop(p->code.u.tripleOP.op2);
            fprintf(f, "%s := %s / %s\n", r, op1, op2);
        }
        break;
        case GETADDR:
        {
            char *l = printop(p->code.u.doubleOP.left);
            char *r = printop(p->code.u.doubleOP.right);
            fprintf(f, "%s := &%s\n", l, r);
            break;
        }
        case GETVAL:
        {
            char *l = printop(p->code.u.doubleOP.left);
            char *r = printop(p->code.u.doubleOP.right);
            fprintf(f, "%s := *%s\n", l, r);
            break;
        }
        case SETVAL:
        {
            char *l = printop(p->code.u.doubleOP.left);
            char *r = printop(p->code.u.doubleOP.right);
            fprintf(f, "*%s := %s\n", l, r);
            break;
        }
        case COPY:
        {
            char *l = printop(p->code.u.doubleOP.left);
            char *r = printop(p->code.u.doubleOP.right);
            fprintf(f, "*%s := *%s\n", l, r);
            break;
        }
        case GOTO:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "GOTO %s\n", op);
            break;
        }
        case IF:
        {
            char *op1 = printop(p->code.u.ifgotoOP.op1);
            char *op2 = printop(p->code.u.ifgotoOP.op2);
            char *tar = printop(p->code.u.ifgotoOP.label);
            fprintf(f, "IF %s %s %s GOTO %s\n", op1, p->code.u.ifgotoOP.relop, op2, tar);
        }
        break;
        case RETURN:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "RETURN %s\n", op);
            break;
        }
        case DEC:
        {
            // printf("hello\n");
            p->code.u.decOP.op->kind = OP_VARIABLE;
            char *op = printop(p->code.u.decOP.op);
            fprintf(f, "DEC %s %d\n", op, p->code.u.decOP.size);
            break;
        }
        case ARG:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "ARG %s\n", op);
            break;
        }
        case CALL:
        {
            // printf("CALL\n");
            char *left = printop(p->code.u.doubleOP.left);
            // printf("CALL\n");
            p->code.u.doubleOP.right->kind = OP_FUNCTION;
            char *right = printop(p->code.u.doubleOP.right);
            fprintf(f, "%s := CALL %s\n", left, right);
        }
        break;
        case PARAM:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "PARAM %s\n", op);
            break;
        }
        case READ:
        {
            char *op = printop(p->code.u.singleOP.op);
            fprintf(f, "READ %s\n", op);
            break;
        }
        case WRITE:
        {
            char *op = printop(p->code.u.singleOP.op);
            // printf("p->code.u.singleOP.op->kind=%d\n", p->code.u.singleOP.op->kind);
            if (p->code.u.singleOP.op->kind == OP_VARIABLE || p->code.u.singleOP.op->kind == OP_CONSTANT || p->code.u.singleOP.op->kind == OP_TEMP)
                fprintf(f, "WRITE %s\n", op);
            else
                fprintf(f, "WRITE *%s\n", op);
            break;
        }
        case TEST:
        {
            fprintf(f, "TEST \n");
            break;
        }
        default:
            break;
        }
        p = p->next;
        // printf("sdsa");
    }
    fclose(f);
}
void test()
{
    singleOP(LABEL, newLabel());
    singleOP(FUNC, find("main", 1)->op);
    doubleOP(ASSIGN, newTemp(0), newTemp(0));
    doubleOP(GETVAL, newTemp(0), newConst(1));
}

void tranProgram(Node *node)
{
    tranExtDefList(node->children[0]);
}

void tranExtDefList(Node *node)
{
    if (node->numChildren == 2) // ExtDef ExtDefList
    {
        tranExtDef(node->children[0]);
        tranExtDefList(node->children[1]);
    }
}
void tranExtDef(Node *node)
{
    switch (node->numChildren)
    {
    case 2: // Specifier SEMI
        break;
    case 3: // Specifier FunDec CompSt
        // printf("Specifier FunDec CompSt\n");
        tranFunDec(node->children[1]);
        tranCompSt(node->children[2]);
        break;
    default:
    {
        // printf("unexpected!\n");
        break;
    }
    }
}
void tranFunDec(Node *node)
{
    // printf("tranFunDec\n");
    Operand func = (Operand)malloc(sizeof(Operand_));
    func->kind = OP_FUNCTION;
    func->u.name = node->children[0]->val;
    singleOP(FUNC, func);
    switch (node->numChildren)
    {
    case 4: // ID LP VarList RP
        tranVarList(node->children[2], 1);
        break;
    case 3: // ID LP RP
        break;
    default:
    {
        // printf("unexpected!\n");
        break;
    }
    }
}

void tranCompSt(Node *node)
{
    // printf("tranCompSt\n");
    tranDefList(node->children[1]);
    tranStmtList(node->children[2]);
}
void tranVarDec(Node *node, int isFunc)
{
    assert(node != NULL);
    switch (node->numChildren)
    {
    case 1: // ID
    {
        if (isFunc)
        {

            FieldList res = find(node->children[0]->val, 0);
            res->op->kind = OP_VARIABLE;
            singleOP(PARAM, res->op);
        }
    }
    break;
    case 4: // VarDec LB INT RB
    {
        Node *p = node->children[0]->children[0];
        if (strcmp(p->label, "ID") != 0)
        {
            p = p->children[0];
        }
        // PrintNode(p, 0);
        FieldList res = find(p->val, 0);
        // printf("size=%d\n", res->size);
        // printf("kind=%d\n", res->op->kind);
        decOP(res->op, res->size);
    }
    break;
    default:
    {
        printf("unexpected!486\n");
        break;
    }
    }
}
void tranVarList(Node *node, int isFunc)
{
    tranParamDec(node->children[0], isFunc);
    switch (node->numChildren)
    {
    case 3: // ParamDec COMMA VarList
    {
        tranVarList(node->children[2], isFunc);
    }
    break;
    case 1: // ParamDec
        break;
    default:
    {
        // printf("unexpected!\n");
        break;
    }
    }
}
void tranParamDec(Node *node, int isFunc)
{
    tranVarDec(node->children[1], isFunc);
}
void tranStmtList(Node *node)
{
    if (node->numChildren == 2)
    {
        tranStmt(node->children[0]);
        tranStmtList(node->children[1]);
    }
}
void tranDefList(Node *node)
{
    // printf("out\n");
    // PrintNode(node,0);
    if (node->numChildren == 2) // Def DefList
    {
        // printf("in\n");
        tranDef(node->children[0]);
        // printf("in\n");
        tranDefList(node->children[1]);
    }
}
void tranDef(Node *node)
{
    tranDecList(node->children[1]);
}
void tranDecList(Node *node)
{
    tranDec(node->children[0]);
    switch (node->numChildren)
    {
    case 1: // Dec
        break;
    case 3: // Dec COMMA DecList
    {
        tranDecList(node->children[2]);
    }
    break;
    default:
    {
        // printf("unexpected!\n");
        break;
    }
    }
}

void tranStmt(Node *node)
{
    // printf("stmt\n");
    // PrintNode(node, 0);
    switch (node->numChildren)
    {
    case 2: // Exp SEMI
        tranExp(node->children[0], NULL, node);
        break;
    case 1: // CompSt
        tranCompSt(node->children[0]);
        break;
    case 3: // RETURN Exp SEMI
    {
        Operand t1 = newTemp(0);
        tranExp(node->children[1], t1, node);
        singleOP(RETURN, t1);
    }
    break;
    case 5:
    {
        if (strcmp(node->children[0]->label, "IF") == 0) // IF LP Exp RP Stmt
        {
            Operand label1 = newLabel();
            Operand label2 = newLabel();
            tranCond(node->children[2], label1, label2);
            singleOP(LABEL, label1);
            tranStmt(node->children[4]);
            singleOP(LABEL, label2);
        }
        else // WHILE LP Exp RP Stmt
        {
            Operand label1 = newLabel();
            Operand label2 = newLabel();
            Operand label3 = newLabel();
            singleOP(LABEL, label1);
            tranCond(node->children[2], label2, label3);
            singleOP(LABEL, label2);
            tranStmt(node->children[4]);
            singleOP(GOTO, label1);
            singleOP(LABEL, label3);
        }
    }
    break;
    case 7: // IF LP Exp RP Stmt ELSE Stmt
    {
        Operand label1 = newLabel();
        Operand label2 = newLabel();
        Operand label3 = newLabel();
        tranCond(node->children[2], label1, label2);
        singleOP(LABEL, label1);
        tranStmt(node->children[4]);
        singleOP(GOTO, label3);
        singleOP(LABEL, label2);
        tranStmt(node->children[6]);
        singleOP(LABEL, label3);
    }
    break;
    default:
    {
        // printf("unexpected!\n");
        break;
    }
    }
}
void tranDec(Node *node)
{
    // printf("dec\n");
    tranVarDec(node->children[0], 0);
    // PrintNode(node, 0);
    switch (node->numChildren)
    {
    case 1: // VarDec
        break;
    case 3: // VarDec ASSIGNOP Exp
    {
        if (node->children[0]->numChildren == 1)
        {
            // printf("in ID\n");
            //  PrintNode(node, 0);
            FieldList res = find(node->children[0]->children[0]->val, 0);
            Operand t1 = newTemp(0);
            tranExp(node->children[2], t1, node);
            doubleOP(ASSIGN, res->op, t1);
        }
        else
        {
            /*printf("now\n");
            Operand t1 = newTemp(1);
            tranExp(node->children[2], t1, node);
            FieldList res = find(node->children[0]->children[0]->children[0]->val, 0);
*/
        }
    }
    break;
    default:
    {
        // printf("unexpected!\n");
        break;
    }
    }
}
void tranExp(Node *node, Operand place, Node *from)
{
    // printNode(node);
    //    t("s");
    assert(node != NULL);
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp

    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT

    // Exp -> LP Exp RP
    if (!strcmp(node->children[0]->label, "LP"))
        tranExp(node->children[1], place, node);

    else if (!strcmp(node->children[0]->label, "Exp") ||
             !strcmp(node->children[0]->label, "NOT"))
    {
        // 条件表达式 和 基本表达式
        if (strcmp(node->children[1]->label, "LB") &&
            strcmp(node->children[1]->label, "DOT"))
        {
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | NOT Exp
            if (!strcmp(node->children[1]->label, "AND") ||
                !strcmp(node->children[1]->label, "OR") ||
                !strcmp(node->children[1]->label, "RELOP") ||
                !strcmp(node->children[0]->label, "NOT"))
            {
                Operand label1 = newLabel();
                Operand label2 = newLabel();
                Operand true_num = newConst(1);
                Operand false_num = newConst(0);
                doubleOP(ASSIGN, place, false_num);
                tranCond(node, label1, label2);
                singleOP(LABEL, label1);
                doubleOP(ASSIGN, place, true_num);
            }
            else
            {
                // printNode(node->children[0]->children[1]);
                if (!strcmp(node->children[1]->label, "ASSIGNOP"))
                {
                    // printf("assign\n");
                    if (node->children[0]->numChildren == 4 && (!strcmp(node->children[0]->children[1]->label, "LB")) && node->children[2]->numChildren == 4 && (!strcmp(node->children[2]->children[1]->label, "LB")))
                    {
                        // printf("lr\n");
                        Node *p = node->children[0]->children[0];
                        if (strcmp(p->label, "ID") != 0)
                        {
                            p = p->children[0];
                        }
                        FieldList res = find(p->val, 0);

                        Operand wid = newConst(4);
                        Operand base = newTemp(0);
                        // Operand offset = newTemp(0);
                        Operand addr = newTemp(0);
                        Operand temp = newTemp(0);
                        // FieldList res = find(from->children[2]->children[0]->children[0]->val, 0);
                        // printNode(node);
                        doubleOP(GETADDR, base, res->op);
                        // offset
                        int off = atoi(node->children[0]->children[2]->children[0]->val);
                        Operand offset = newConst(off);

                        tripleOP(MUL, temp, wid, offset);
                        tripleOP(ADD, addr, temp, base);
                        //
                        Node *pt = node->children[2]->children[0];
                        if (strcmp(pt->label, "ID") != 0)
                        {
                            pt = pt->children[0];
                        }
                        FieldList rest = find(pt->val, 0);

                        // Operand wid = newConst(4);
                        Operand baset = newTemp(0);
                        // Operand offset = newTemp(0);
                        Operand addrt = newTemp(0);
                        Operand tempt = newTemp(0);
                        // FieldList res = find(from->children[2]->children[0]->children[0]->val, 0);
                        // printNode(node);
                        doubleOP(GETADDR, baset, rest->op);
                        // offset
                        int offt = atoi(node->children[2]->children[2]->children[0]->val);
                        Operand offsett = newConst(offt);

                        tripleOP(MUL, tempt, wid, offsett);
                        tripleOP(ADD, addrt, tempt, baset);

                        doubleOP(COPY, addr, addrt);

                        return;
                    }
                    else if (node->children[0]->numChildren == 4 && (!strcmp(node->children[0]->children[1]->label, "LB"))) // array
                    {
                        // printf("l\n");
                        Node *p = node->children[0]->children[0];
                        if (strcmp(p->label, "ID") != 0)
                        {
                            p = p->children[0];
                        }
                        FieldList res = find(p->val, 0);

                        Operand wid = newConst(4);
                        Operand base = newTemp(0);
                        // Operand offset = newTemp(0);
                        Operand addr = newTemp(0);
                        Operand temp = newTemp(0);
                        // FieldList res = find(from->children[2]->children[0]->children[0]->val, 0);
                        // printNode(node);
                        doubleOP(GETADDR, base, res->op);
                        // offset
                        int off = atoi(node->children[0]->children[2]->children[0]->val);
                        Operand offset = newConst(off);

                        tripleOP(MUL, temp, wid, offset);
                        tripleOP(ADD, addr, temp, base);
                        Operand val = newTemp(0);
                        tranExp(node->children[2], val, node);
                        doubleOP(SETVAL, addr, val);

                        return;
                    }
                    else if (node->children[2]->numChildren == 4 && (!strcmp(node->children[2]->children[1]->label, "LB")))
                    {
                        // printf("r\n");
                        Node *p = node->children[2]->children[0];
                        if (strcmp(p->label, "ID") != 0)
                        {
                            p = p->children[0];
                        }
                        FieldList res = find(p->val, 0);

                        Operand wid = newConst(4);
                        Operand base = newTemp(0);
                        // Operand offset = newTemp(0);
                        Operand addr = newTemp(0);
                        Operand temp = newTemp(0);
                        // FieldList res = find(from->children[2]->children[0]->children[0]->val, 0);
                        // printNode(node);
                        doubleOP(GETADDR, base, res->op);
                        // offset
                        int off = atoi(node->children[2]->children[2]->children[0]->val);
                        Operand offset = newConst(off);

                        tripleOP(MUL, temp, wid, offset);
                        tripleOP(ADD, addr, temp, base);

                        // doubleOP(GETVAL, val, addr);
                        FieldList vres = find(node->children[0]->children[0]->val, 0);
                        // printf("assign\n");
                        // Operand t1 = newTemp(0);
                        // tranExp(node->children[2], t1, node);
                        // printf("assign\n");
                        doubleOP(GETVAL, vres->op, addr);

                        return;
                    }
                    else if (strcmp(node->children[0]->children[0]->label, "ID") != 0)

                    {
                        // printf("456\n");
                        // PrintNode(node, 0);
                        //  printf("node->children[0]->children[0]->label=%s\n", node->children[0]->children[0]->label);
                        Node *p = node->children[0]->children[0];
                        if (strcmp(p->label, "ID") != 0)
                        {
                            p = p->children[0];
                        }
                        FieldList res = find(p->val, 0);
                        // printf("sadsize=%d\n", res->type->kind);
                        Operand t1 = newTemp(0);
                        tranExp(node->children[2], t1, node);
                        doubleOP(SETVAL, res->op, t1);
                    }
                    // printf("", node->children[0]->);
                    //  PrintNode(node->children[0]->children[0], 0);
                    else
                    {
                        FieldList res = find(node->children[0]->children[0]->val, 0);
                        // printf("assign\n");
                        Operand t1 = newTemp(0);
                        tranExp(node->children[2], t1, node);
                        // printf("assign\n");
                        doubleOP(ASSIGN, res->op, t1);
                        // printf("assign\n");
                    }
                }
                else
                {
                    Operand t1 = newTemp(0);
                    tranExp(node->children[0], t1, node);
                    Operand t2 = newTemp(0);
                    tranExp(node->children[2], t2, node);
                    // Exp -> Exp PLUS Exp
                    if (!strcmp(node->children[1]->label, "PLUS"))
                    {
                        tripleOP(ADD, place, t1, t2);
                    }
                    // Exp -> Exp MINUS Exp
                    else if (!strcmp(node->children[1]->label, "MINUS"))
                    {
                        tripleOP(SUB, place, t1, t2);
                    }
                    // Exp -> Exp STAR Exp
                    else if (!strcmp(node->children[1]->label, "STAR"))
                    {
                        tripleOP(MUL, place, t1, t2);
                    }
                    // Exp -> Exp DIV Exp
                    else if (!strcmp(node->children[1]->label, "DIV"))
                    {
                        tripleOP(DIV, place, t1, t2);
                    }
                }
            }
        }
        // 数组访问
        /*else
        {
            printf("sdada\n");
            Operand wid = newConst(4);
            Operand base = newTemp(0);
            Operand offset = newTemp(0);
            Operand addr = newTemp(0);
            Operand temp = newTemp(0);
            FieldList res = find(from->children[2]->children[0]->children[0]->val, 0);

            //doubleOP(GETADDR, base, res->op);
            //tranExp(from->children[2]->children[2], offset, node);
            //tripleOP(MUL, temp, wid, offset);
            //tripleOP(ADD, addr, temp, base);
            // FieldList left = find(from->children[0]->children[0]->val, 0);
            // printf("",place->)
            //doubleOP(GETVAL, place, addr);
            // PrintNode(from, 0);
            // Operand left = newTemp(0);
        }*/
    }
    // 单目运算符
    //  Exp -> MINUS Exp
    else if (!strcmp(node->children[0]->label, "MINUS"))
    {
        Operand t1 = newTemp(0);
        tranExp(node->children[1], t1, node);
        // Operand zero = newConst(0);
        tripleOP(SUB, place, zero, t1);
    }

    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(node->children[0]->label, "ID") && node->numChildren > 1)
    {
        // printf("// Exp -> ID LP Args RP | ID LP RP\n");
        // printf("node->children[0]->val=%s\n", node->children[0]->val);
        Operand funcTemp = find(node->children[0]->val, 1)->op;
        // Exp -> ID LP Args RP
        if (!strcmp(node->children[2]->label, "Args"))
        {

            FieldList function = find(node->children[0]->val, 1);
            assert(function != NULL);
            // t("a");
            // assert(function->type!= NULL);
            // printf("function->type->kind=%s\n", function->name);
            // t("a");
            int argCount = function->type->u.function.number_of_params;
            // t("a");
            Operand arg_list[argCount];
            tranArgs(node->children[2], arg_list, argCount - 1);
            // t("a");
            if (!strcmp(function->name, "write"))
            {
                singleOP(WRITE, arg_list[0]);
                if (place)
                {
                    doubleOP(ASSIGN, place, zero);
                }
            }
            else
            {
                for (int i = 0; i < argCount; ++i)
                {
                    singleOP(ARG, arg_list[i]);
                }
                Operand func = (Operand)malloc(sizeof(Operand_));
                func->kind = FUNC;
                func->u.name = function->name;
                if (place)
                    doubleOP(CALL, place, func);
                else
                {
                    Operand t1 = newTemp(0);
                    doubleOP(CALL, t1, func);
                }
            }
        }
        // Exp -> ID LP RP
        else
        {
            // printf("Exp -> ID LP RP node->children[0]->val=%s\n", node->children[0]->val);
            if (!strcmp(node->children[0]->val, "read"))
            {
                // printf("READ\n");
                singleOP(READ, place);
            }
            else
            {
                if (place)
                {
                    doubleOP(CALL, place, funcTemp);
                }
                else
                {
                    Operand temp = newTemp(0);
                    doubleOP(CALL, temp, funcTemp);
                }
            }
        }
    }
    // Exp -> ID
    else if (!strcmp(node->children[0]->label, "ID"))
    {
        // PrintNode(node, 0);
        // printf("node->children[0]->val=%s\n", node->children[0]->val);
        FieldList res = find(node->children[0]->val, 0);
        assert(res != 0);
        doubleOP(ASSIGN, place, res->op);
    }
    else // int
    {
        int val = atoi(node->children[0]->val);
        Operand op = newConst(val);
        doubleOP(ASSIGN, place, op);
    }
}

void tranArgs(Node *node, Operand arg_list[], int index)
{
    switch (node->numChildren)
    {
    case 3: // Exp COMMA Args
    {
        tranArgs(node->children[2], arg_list, index - 1);
        Operand t1 = newTemp(0);
        tranExp(node->children[0], t1, node);
        arg_list[index] = t1;
    }
    break;
    case 1: // Exp
    {
        Operand t1 = newTemp(0);
        tranExp(node->children[0], t1, node);
        arg_list[index] = t1;
    }
    break;
    default:
        break;
    }
}
void tranCond(Node *node, Operand labelTrue, Operand labelFalse)
{
    assert(node != NULL);
    if (!strcmp(node->children[0]->label, "NOT"))
    {
        tranCond(node->children[1], labelFalse, labelTrue);
    }
    // Exp -> Exp RELOP Exp
    else if (!strcmp(node->children[1]->label, "RELOP"))
    {
        Operand t1 = newTemp(0);
        Operand t2 = newTemp(0);
        tranExp(node->children[0], t1, node);
        tranExp(node->children[2], t2, node);
        ifgotoOP(labelTrue, t1, t2, node->children[1]->val);
        singleOP(GOTO, labelFalse);
    }
    // Exp -> Exp AND Exp
    else if (!strcmp(node->children[1]->label, "AND"))
    {
        Operand label1 = newLabel();
        tranCond(node->children[0], label1, labelFalse);
        singleOP(LABEL, label1);
        tranCond(node->children[2], labelTrue, labelFalse);
    }
    // Exp -> Exp OR Exp
    else if (!strcmp(node->children[1]->label, "OR"))
    {
        Operand label1 = newLabel();
        tranCond(node->children[0], labelTrue, label1);
        singleOP(LABEL, label1);
        tranCond(node->children[2], labelTrue, labelFalse);
    }
    // other cases
    else
    {
        Operand t1 = newTemp(0);
        tranExp(node, t1, node);
        ifgotoOP(t1, zero, labelTrue, "!=");
        singleOP(GOTO, labelFalse);
    }
}

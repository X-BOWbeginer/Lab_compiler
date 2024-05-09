#include "target.h"
extern InterCodes head;
int changedVar;
InterCodes pp;
void loadValue(FILE *f, Operand op, char *reg)
{
    if (op->kind == OP_CONSTANT)
    {
        fprintf(f, "  li %s, %d\n", reg, op->u.value);
    }
    else
    {
        // int offset = getoffset(op);
        fprintf(f, "  lw %s, -%d($fp)\n", reg, getoffset(op));
    }
}

void storeValue(FILE *f, int offset, char *reg)
{
    fprintf(f, "  sw %s, -%d($fp)\n", reg, offset);
}
//
int getoffset(Operand op)
{
    int off = op->u.var_no;
    if (op->kind == OP_TEMP)
        off += varCnt;
    return off * 4;
}
void writeback(FILE *f)
{
    if (changedVar != -1)
    {
        storeValue(f, changedVar, "$t0");
        changedVar = -1;
    }
}
//

void handleAssign(FILE *f, Operand leftOP, Operand rightOP)
{
        loadValue(f, rightOP, "$t1");
        fprintf(f, "  move $t0, $t1\n");
}

void handleGetAddr(FILE *f, Operand rightOP)
{
    fprintf(f, "  addi $t0, $fp, -%d\n", getoffset(rightOP));
}

void handleGetVal(FILE *f, Operand rightOP)
{

    loadValue(f, rightOP, "$t1");
    fprintf(f, "  lw $t0, 0($t1)\n");
}

void handleSetVal(FILE *f, Operand leftOP, Operand rightOP)
{
    loadValue(f, leftOP, "$t0");
    loadValue(f, rightOP, "$t1");
    fprintf(f, "  sw $t1, 0($t0)\n");
}

void handleCopy(FILE *f, Operand leftOP, Operand rightOP)
{
    loadValue(f, leftOP, "$t0");
    loadValue(f, rightOP, "$t1");
    fprintf(f, "  lw $t1, 0($t1)\n");
    fprintf(f, "  sw $t1, 0($t0)\n");
}

void handleAdd(FILE *f, Operand op1, Operand op2)
{

    loadValue(f, op1, "$t1");
    loadValue(f, op2, "$t2");
    fprintf(f, "  add $t0, $t1, $t2\n");
}

void handleSub(FILE *f, Operand op1, Operand op2)
{
    loadValue(f, op1, "$t1");
    loadValue(f, op2, "$t2");
    fprintf(f, "  sub $t0, $t1, $t2\n");
}

void handleMul(FILE *f, Operand op1, Operand op2)
{

    loadValue(f, op1, "$t1");
    loadValue(f, op2, "$t2");
    fprintf(f, "  mul $t0, $t1, $t2\n");
}

void handleDiv(FILE *f, Operand op1, Operand op2)
{

    loadValue(f, op1, "$t1");
    loadValue(f, op2, "$t2");
    fprintf(f, "  div $t1, $t2\n  mflo $t0\n");
}
void handleIf(FILE *f, Operand op1, Operand op2, char *relop, char *tar)
{
    loadValue(f, op1, "$t1");

    loadValue(f, op2, "$t2");

    if (!strcmp("==", relop))
        fprintf(f, "  beq $t1, $t2, %s\n", tar);
    else if (!strcmp("!=", relop))
        fprintf(f, "  bne $t1, $t2, %s\n", tar);
    else if (!strcmp(">", relop))
        fprintf(f, "  bgt $t1, $t2, %s\n", tar);
    else if (!strcmp("<", relop))
        fprintf(f, "  blt $t1, $t2, %s\n", tar);
    else if (!strcmp(">=", relop))
        fprintf(f, "  bge $t1, $t2, %s\n", tar);
    else if (!strcmp("<=", relop))
        fprintf(f, "  ble $t1, $t2, %s\n", tar);
}

void handleReturn(FILE *f, Operand ret)
{

    loadValue(f, ret, "$v0");

    fprintf(f, "  addi $sp, $fp, 4\n  lw $fp, 0($fp)\n");
    fprintf(f, "  jr $ra\n");
}

void handleArg(FILE *f, Operand op)
{
    fprintf(f, "  addi $sp, $sp, -4\n");

    loadValue(f, op, "$t0");

    fprintf(f, "  sw $t0, 0($sp)\n");
}

void handleCall(FILE *f, Operand res, Operand op)
{
    fprintf(f, "  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n");

    fprintf(f, "  jal %s\n", printop(op));
    fprintf(f, "  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n");
    int resoffset = getoffset(res);
    storeValue(f, resoffset, "$v0");
    FieldList func = find(op->u.name, 1);
    int argc = func->type->u.function.number_of_params;
    fprintf(f, "  addi $sp, $sp, %d\n", argc * 4);
}

void handleParam(FILE *f, InterCodes pp, Operand op)
{
    FieldList func = find(pp->prev->code.u.singleOP.op->u.name, 1);
    int argc = func->type->u.function.number_of_params;

    for (int i = 0; i < argc; i++)
    {
        Operand op = pp->code.u.singleOP.op;
        fprintf(f, "  lw $t0, %d($fp)\n", 8 + i * 4);
        storeValue(f, getoffset(op), "$t0");
        pp = pp->next;
    }
}
//

void Gencode(FILE *f, InterCodes p)
{
    if (p == NULL)
        return;
    else
    {
        InterCode code = &p->code;
        // printf("111 p->code.kind=%d\n", p->code.kind);
        // printf("%s\n", printop(p->code.u.singleOP.op));
        switch (p->code.kind)
        {
        case FUNC:
        {

            fprintf(f, "\n%s:\n", p->code.u.singleOP.op->u.name);
            fprintf(f, "  addi $sp, $sp, -4\n  sw $fp, 0($sp)\n  move $fp, $sp\n");
            fprintf(f, "  addi $sp, $sp, -%d\n", (tmpCnt + varCnt) * 4);
            break;
        }
        case LABEL:
        {
            fprintf(f, "label%d:\n", code->u.singleOP.op->u.var_no);
            break;
        }
        case GOTO:
        {
            fprintf(f, "  j %s\n", printop(p->code.u.singleOP.op));
            break;
        }
        case ASSIGN:
        case GETADDR:
        case GETVAL:
        case SETVAL:
        case COPY:
        {
            Operand leftOP = code->u.doubleOP.left;
            Operand rightOP = code->u.doubleOP.right;
            changedVar = getoffset(leftOP);
            switch (p->code.kind)
            {
            case ASSIGN:
                handleAssign(f, leftOP, rightOP);
                break;
            case GETADDR:
                handleGetAddr(f, rightOP);
                break;
            case GETVAL:
                handleGetVal(f, rightOP);
                break;
            case SETVAL:
                handleSetVal(f, leftOP, rightOP);
                break;
            case COPY:
                handleCopy(f, leftOP, rightOP);
                break;
            }

            writeback(f);
            break;
        }
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        {
            Operand result = p->code.u.tripleOP.result;
            Operand op1 = p->code.u.tripleOP.op1;
            Operand op2 = p->code.u.tripleOP.op2;
            changedVar = getoffset(result);

            switch (p->code.kind)
            {
            case ADD:
                handleAdd(f, op1, op2);
                break;
            case SUB:
                handleSub(f, op1, op2);
                break;
            case MUL:
                handleMul(f, op1, op2);
                break;
            case DIV:
                handleDiv(f, op1, op2);
                break;
            }

            writeback(f);
            break;
        }
        case IF:
        {
            Operand op1 = p->code.u.ifgotoOP.op1;
            Operand op2 = p->code.u.ifgotoOP.op2;
            char *relop = p->code.u.ifgotoOP.relop;
            char *tar = printop(p->code.u.ifgotoOP.label);

            handleIf(f, op1, op2, relop, tar);
            break;
        }
        case RETURN:
        {
            Operand ret = p->code.u.singleOP.op;
            handleReturn(f, ret);
            break;
        }
        case ARG:
        {
            Operand op = p->code.u.singleOP.op;
            handleArg(f, op);
            break;
        }
        case CALL:
        {
            Operand res = p->code.u.doubleOP.left;
            Operand op = p->code.u.doubleOP.right;
            handleCall(f, res, op);
            break;
        }
        case PARAM:
        {
            Operand op = p->code.u.singleOP.op;
            handleParam(f, pp, op);
            break;
        }
        case READ:
        {
            Operand op = p->code.u.singleOP.op;
            fprintf(f, "  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal read\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n");
            storeValue(f, getoffset(op), "$v0");
            // fprintf(f, "#lalala%s\n", printop(op));
            break;
        }
        case WRITE:
        {
            Operand op = p->code.u.singleOP.op;
            loadValue(f, op, "$a0");
            fprintf(f, "  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal write\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n");
            break;
        }

        default:
            break;
        }
    }
}
void WriteTarget(FILE *f)
{
    fprintf(f, ".data\n");
    fprintf(f, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, ".globl main\n");
    fprintf(f, ".text\n\n");

    fprintf(f, "read:\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _prompt\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 5\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  jr $ra\n\n");

    fprintf(f, "write:\n");
    fprintf(f, "  li $v0, 1\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _ret\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  move $v0, $0\n");
    fprintf(f, "  jr $ra\n\n");

    pp = head->next;
    // printf("%d,%d\n", varCnt, tmpCnt);
    changedVar = -1;
    while (pp != NULL)
    {
        Gencode(f, pp);
        pp = pp->next;
    }
}
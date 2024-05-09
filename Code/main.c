#include "node.h"
#include "syntax.tab.h"
#include "semantic.h"
// #include "inter.h"

extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE *);
extern void initInterCodes();
extern void writeInterCodes(const char *fielname);
extern void test();
extern void tranProgram();
extern void WriteTarget(FILE *f);
extern void optimize(char *input,char* output);
extern int lab3error;
int ecode = 0;

extern Node *Program;

int main(int argc, char **argv)
{
    /*lab1-4
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    // yydebug=1;
    yyparse();
    if (ecode == 0)
    {
        // PrintNode(Program, 0);
        //  printf("%d\n", Program->numChildren);
        initHashtable();
        program(Program);
        // setVariable();
        // setFunction();
        initInterCodes();
        // test();
        //  printf("ALL\n");
        //  AllSymbol();
        if (lab3error == 0)
        {
            tranProgram(Program);
            writeInterCodes("out1.ir");
        }
        else
        {
            printf("Cannot translate: Code contains variables or parameters of structure type.\n");
        }
        FILE *f = fopen(argv[2], "w");
        WriteTarget(f);
    }*/
    //lab 5
    optimize(argv[1], argv[2]);

    return 0;
}

%{
#include "node.h"
#include "syntax.tab.h"

int yycolumn = 1;
extern int ecode;

char inputBuffStr[32];

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

%option yylineno


digit [0-9]
letter [_a-zA-Z]
ws [ \t]
lines \n

semi ";"
comma ","
assignop "="
relop >|<|>=|<=|==|!=
plus  "+"
minus  "-"
star  "*"
div  "/"
and  "&&"
or  "||"
dot  "."
not  "!"
type  int|float
lp  "("
rp  ")"
lb  "["
rb  "]"
lc  "{"
rc  "}"
struct  struct
return  return
if  if
else  else
while  while
int ([1-9][0-9]*|0)
float ([1-9][0-9]*\.[0-9]+)
id [A-Za-z_][A-Za-z_0-9]*
%%
"//" {char c = input();while (c != '\n') c = input();}

"/*" {
	char c1 = input();
    char c2 = input();
	while (c2 && !(c1 == '*' && c2 == '/')) {
       // printf("%c", c1);
		c1 = c2;
		c2 = input();
	}
	if (!c2) {
		printf("Error type B at line %d: comment format error.\n", yylineno);
	}
}

{ws}+ {}
{lines} {yycolumn = 1;}
{semi} {strcpy(inputBuffStr,yytext);yylval.node=createNode("SEMI","t",yytext,yylineno);return SEMI;}
{comma} {strcpy(inputBuffStr,yytext);yylval.node=createNode("COMMA","t",yytext,yylineno);return COMMA;}

{assignop} {strcpy(inputBuffStr,yytext);yylval.node=createNode("ASSIGNOP","t",yytext,yylineno);return ASSIGNOP;}
{relop} {strcpy(inputBuffStr,yytext);yylval.node=createNode("RELOP","relop",yytext,yylineno);return RELOP;}
{plus}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("PLUS","t",yytext,yylineno);return PLUS;}
{minus}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("MINUS","t",yytext,yylineno);return MINUS;}
{star}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("STAR","t",yytext,yylineno);return STAR;}
{div}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("DIV","t",yytext,yylineno);return DIV;}
{and}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("AND","t",yytext,yylineno);return AND;}
{or}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("OR","t",yytext,yylineno);return OR;}
{dot} {strcpy(inputBuffStr,yytext);yylval.node=createNode("DOT","t",yytext,yylineno);return DOT;}
{not} {strcpy(inputBuffStr,yytext);yylval.node=createNode("NOT","t",yytext,yylineno);return NOT;}
{type}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("TYPE","type",inputBuffStr,yylineno);return TYPE;}
{lp}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("LP","t",yytext,yylineno);return LP;}
{rp}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("RP","t",yytext,yylineno);return RP;}
{lb}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("LB","t",yytext,yylineno);return LB;}
{rb}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("RB","t",yytext,yylineno);return RB;}
{lc}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("LC","t",yytext,yylineno);return LC;}
{rc}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("RC","t",yytext,yylineno);return RC;}
{struct}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("STRUCT","t",yytext,yylineno);return STRUCT;}
{return}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("RETURN","t",yytext,yylineno);return RETURN;}  
{if}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("IF","t",yytext,yylineno);return IF;}
{else}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("ELSE","t",yytext,yylineno);return ELSE;}
{while}  {strcpy(inputBuffStr,yytext);yylval.node=createNode("WHILE","t",yytext,yylineno);return WHILE;} 
{int} {strcpy(inputBuffStr,yytext);yylval.node=createNode("INT","int",inputBuffStr,yylineno);return INT;}
{float} {strcpy(inputBuffStr,yytext);yylval.node=createNode("FLOAT","float",yytext,yylineno);return FLOAT;}
{id} {strcpy(inputBuffStr,yytext);yylval.node=createNode("ID","id",inputBuffStr,yylineno);return ID;}

(\.{digit}+)|({digit}+\.) {ecode=1;printf("Error type A at Line %d: illegal float number \'%s\'\n",yylineno, yytext);}
.  {ecode=1;printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}

%%
//NOLINTEND
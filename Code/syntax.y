%locations

%{

#include"lex.yy.c"
#include "node.h"
int yylex(void);
int yyerror(const char* msg);
int yyerrstatus=0;
extern int ecode;
Node* Program;

%}
%define parse.error verbose
%union{
     Node* node;
}



/* declared tokens */

%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> PLUS
%token <node> MINUS
%token <node> STAR
%token <node> DIV
%token <node> AND
%token <node> OR
%token <node> DOT
%token <node> NOT
%token <node> TYPE
%token <node> LP
%token <node> RP
%token <node> LB
%token <node> RB
%token <node> LC
%token <node> RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> INT
%token <node> FLOAT
%token <node> SEMI
%token <node> ID
/* declared types */
%type <node> Program ExtDefList ExtDef Specifier ExtDecList FunDec CompSt VarDec StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Exp Stmt Def DecList Dec Args
//conflict
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left DIV STAR
%right NOT
%left LB RB LP RP DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
//High-level Definitions
Program : ExtDefList    {$$=createNode("Program","nt",NULL,@1.first_line);addChild($$,$1);Program=$$;}
;
ExtDefList : ExtDef ExtDefList {$$=createNode("ExtDefList","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
| {$$=createNode("ExtDefList","null",NULL,@0.first_line);}//{$$=NULL;}
;
ExtDef : Specifier ExtDecList SEMI {$$=createNode("ExtDef","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}//global def
| Specifier SEMI {$$=createNode("ExtDef","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}//struct
| Specifier FunDec CompSt {$$=createNode("ExtDef","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}//function
| Specifier FunDec SEMI{$$=createNode("ExtDef","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}//added in lab2
| error SEMI  {ecode=1;}
| Specifier error  {ecode=1;}

;
ExtDecList : VarDec {$$=createNode("ExtDecList","nt",NULL,@1.first_line);addChild($$,$1);}
| VarDec COMMA ExtDecList {$$=createNode("ExtDecList","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
;
//Specifiers
Specifier : TYPE {$$=createNode("Specifier","nt",NULL,@1.first_line);addChild($$,$1);}
| StructSpecifier {$$=createNode("Specifier","nt",NULL,@1.first_line);addChild($$,$1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=createNode("StructSpecifier","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);}
| STRUCT Tag {$$=createNode("StructSpecifier","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
;
OptTag : ID {$$=createNode("OptTag","nt",NULL,@1.first_line);addChild($$,$1);}
|  {$$=createNode("OptTag","null",NULL,@0.first_line);}//{$$=NULL;}
;
Tag : ID {$$=createNode("Tag","nt",NULL,@1.first_line);addChild($$,$1);}
; 
//Declarators
VarDec : ID {$$=createNode("VarDec","nt",NULL,@1.first_line);addChild($$,$1);}//varabie
| VarDec LB INT RB {$$=createNode("VarDec","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
;
FunDec : ID LP VarList RP  {$$=createNode("FunDec","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
| ID LP RP {$$=createNode("FunDec","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
;
VarList : ParamDec COMMA VarList {$$=createNode("VarList","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| ParamDec {$$=createNode("VarList","nt",NULL,@1.first_line);addChild($$,$1);}
;
ParamDec : Specifier VarDec {$$=createNode("ParamDec","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
;
//Statements
CompSt : LC DefList StmtList RC {$$=createNode("CompSt","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
|   error RC  {ecode=1;}
;
StmtList : Stmt StmtList {$$=createNode("StmtList","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
| {$$=createNode("StmtList","null",NULL,@0.first_line);}//{$$=NULL;}
;
Stmt : Exp SEMI {$$=createNode("Stmt","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
| CompSt {$$=createNode("Stmt","nt",NULL,@1.first_line);addChild($$,$1);}
| RETURN Exp SEMI {$$=createNode("Stmt","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=createNode("Stmt","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);}
| IF LP Exp RP Stmt ELSE Stmt {$$=createNode("Stmt","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);addChild($$,$6);addChild($$,$7);}
| WHILE LP Exp RP Stmt {$$=createNode("Stmt","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);}
| error SEMI                    {ecode=1;}
;

//Local Definitions
DefList : Def DefList {$$=createNode("DefList","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
|{$$=createNode("DefList","null",NULL,@0.first_line);}//{$$=NULL;}
;
Def : Specifier DecList SEMI {$$=createNode("Def","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Specifier error SEMI  {ecode=1;}
;
DecList : Dec {$$=createNode("DecList","nt",NULL,@1.first_line);addChild($$,$1);}
| Dec COMMA DecList {$$=createNode("DecList","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Dec error   {ecode=1;}
;
Dec : VarDec {$$=createNode("Dec","nt",NULL,@1.first_line);addChild($$,$1);}
| VarDec ASSIGNOP Exp {$$=createNode("Dec","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| VarDec ASSIGNOP error  {ecode=1;}
;
//Expressions
Exp : Exp ASSIGNOP Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp AND Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp OR Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp RELOP Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp PLUS Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp MINUS Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp STAR Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp DIV Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| LP Exp RP {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| MINUS Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
| NOT Exp {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);}
| ID LP Args RP {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
| ID LP RP {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp LB Exp RB {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
| Exp DOT ID {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| ID {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);}
| INT {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);}
| FLOAT {$$=createNode("Exp","nt",NULL,@1.first_line);addChild($$,$1);}
| error RP  {ecode=1;}
;
Args : Exp COMMA Args {$$=createNode("Args","nt",NULL,@1.first_line);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
| Exp {$$=createNode("Args","nt",NULL,@1.first_line);addChild($$,$1);}
;
%%
int yyerror(const char* msg){
    fprintf(stderr, "Error type B at line %d: %s.\n", yylineno, msg);
}
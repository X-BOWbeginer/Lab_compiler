#include "node.h"
//extern int yylineno;
extern char inputBuffStr[32];
Node* createNode(char* _label, char* _type, char* _val,int _line)
{
    Node* n = malloc(sizeof(Node));
    n->label = _label;
    n->type = _type;// int ,float,id,type,relop,t,nt,null
    //printf("%c\n",_type[0]);
    //printf("%s\n",inputBuffStr);
    strcpy(n->val,inputBuffStr);

    n->line=_line;
    n->children=malloc(sizeof(Node*[10]));//maxsize=7
    return n;
}
void addChild(Node* parent,Node* child)
{
    if(parent!=NULL && child!=NULL)
    {
        parent->children[parent->numChildren]=child;
        parent->numChildren++;
        //parent->line=child->line;
        //printf("%s parent->line(%d)=child->line(%d)\n",parent->label,parent->line,child->line);
    }
    
}
int type_to_int(char* type)
{
    if(strcmp(type,"int")==0)
    return 1;
    else if(strcmp(type,"float")==0)
    return 2;
    else if(strcmp(type,"id")==0)
    return 3;
    else if(strcmp(type,"type")==0)
    return 4;
    else if(strcmp(type,"relop")==0)
    return 5;
    else if(strcmp(type,"t")==0)
    return 6;
    else if(strcmp(type,"nt")==0)
    return 7;
    else 
    return 8;
    
}
void PrintNode(Node* n,int depth)
{
    if(n==NULL)
    return;
    for(int i=0;i<depth;i++)
		{
			printf("  ");
		}
    printf("%s",n->label);

    switch(type_to_int(n->type))
    {
        case 1:{//integer
            printf(": %s",n->val);
            break;
        }
        case 2:{//float
            float val;
            val = atof(n->val);
            printf(": %f",val);
            break;
        }
        case 3:{//id
            printf(": %s",n->val);
            break;
        }
        case 4:{//type
            printf(": %s",n->val);
            break;
        }
        case 5:{//relop
            //printf(" %s",n->val);
            break;
        }
        case 6:{//t
            //printf(" %s",n->val);
            break;
        }
        case 7:{//nt
            printf(" (%d)",n->line);
            break;
        }
        case 8:{//null

            break;
        }
        default:{
            break;
        }
    }

    printf("\n");
    for(int i=0;i<n->numChildren;i++)
    {
        PrintNode(n->children[i],depth+1);
    }
}

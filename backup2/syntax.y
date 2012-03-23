%{
#include<stdio.h>
struct TreeNode {
    int reserveTag;
    double dval;
    char *name;
    struct TreeNode *firstChild;
    struct TreeNode *nextSibling;
    struct TreeNode *ttn;
    int lineno;
};
struct TreeNode *head;
void traverse(struct TreeNode *head, int depth);
void treeSet(struct TreeNode * s0, struct TreeNode * s1, struct TreeNode * s2, struct TreeNode * s3, struct TreeNode * s4);

%}

%union {
    int iVal;
    double dVal;
    struct TreeNode node;
}

/* declared tokens */
%token <node>  SEMI COMMA ASSIGNOP RELOP
%token <node>  AND OR
%token <node>  PLUS MINUS STAR  DIV 
%token <node>  DOT NOT 
%token <node>  LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE ID
%token <iVal>  INT 
%token <dVal>  FLOAT 
%token <node>  TYPE

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  PLUS MINUS
%left   STAR DIV
%right  NOT 
%left  LB RB LP RP DOT

%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec  VarList ParamDec
%type <node> CompSt StmtList Stmt DefList Def DecList Dec Exp Args 
%%

Program     :   ExtDefList  {$$.name = "Program"; treeSet(&$$, &$1, NULL, NULL, NULL); head = $$.ttn; traverse(head, 0);}
            ;
ExtDefList  :   ExtDef  ExtDefList { $$.name ="ExtDefList"; treeSet(&$$, &$1, NULL, NULL, NULL);} 
            |   /* empty */  { $$.name = "ExtDefList Empty"; treeSet(&$$, NULL, NULL, NULL, NULL);}
            ;
ExtDef      :   Specifier   ExtDecList  SEMI   
            |   Specifier   SEMI 
            |   Specifier   FunDec  CompSt {$$.name = "ExtDef"; treeSet(&$$, &$1, &$2, &$3, NULL);}
            ;
ExtDecList  :   VarDec
            |   VarDec  COMMA   ExtDecList
            ;

Specifier   :   TYPE   {$$.name= "SPecifier"; $1.reserveTag = 10; treeSet(&$$, &$1, NULL, NULL, NULL);}
            |   StructSpecifier  { }
            ;
StructSpecifier :   STRUCT  OptTag  LC  DefList RC { }
                |   STRUCT  Tag{ }
                ;
OptTag      :   ID { }
            |   /* empty */
            ;
Tag         :   ID
            ;

VarDec      :   ID { $1.reserveTag = 10; treeSet(&$$, &$1, NULL, NULL, NULL); }
            |   VarDec  LB  INT RB
            ;
FunDec      :   ID  LP  VarList RP
            |   ID  LP  RP  { $1.reserveTag = 10;  treeSet(&$$, &$1, &$2, &$3, NULL); }
            ;
VarList     :   ParamDec    COMMA   VarList
            |   ParamDec
            ;
ParamDec    :   Specifier   VarDec
            ;

CompSt      :   LC  DefList StmtList    RC { $1.reserveTag = 10; treeSet(&$$, &$1, &$2, &$3, &$4); }
            ;
StmtList    :   Stmt    StmtList
            |   
            ;
Stmt        :   Exp SEMI
            |   CompSt
            |   RETURN  Exp SEMI
            |   IF  LP  Exp RP  Stmt  %prec LOWER_THAN_ELSE
            |   IF  LP  Exp RP  Stmt    ELSE    Stmt
            |   WHILE   LP  Exp RP  Stmt
            ;

DefList     :   Def DefList { treeSet(&$$, &$1, &$2, NULL, NULL); }
            |
            ;
Def         :   Specifier   DecList SEMI { $3.reserveTag = 10;  treeSet(&$$, &$1, &$2, &$3, NULL); }
            ;
DecList     :   Dec { treeSet(&$$, &$1, NULL, NULL, NULL); } 
            |   Dec COMMA DecList
            ;
Dec         :   VarDec  {treeSet(&$$, &$1, NULL, NULL, NULL);}
            |   VarDec  ASSIGNOP    Exp
            ;

Exp         :   Exp ASSIGNOP  Exp
            |   Exp AND Exp
            |   Exp OR  Exp
            |   Exp RELOP  Exp
            |   Exp PLUS  Exp
            |   Exp MINUS  Exp
            |   Exp STAR  Exp
            |   Exp DIV  Exp
            |   LP  Exp  RP
            |   MINUS  Exp
            |   NOT   Exp
            |   ID   LP   Args  RP
            |   ID   LP   RP
            |   Exp   LB   Exp  RB
            |   Exp   DOT   ID
            |   ID
            |   INT   
            |   FLOAT
            ;

Args        :   Exp   COMMA   Args
            |   Exp
            ;

%%

#include "lex.yy.c"
int main(int argc, char** argv)
{
    if (argc > 1) {
        FILE *file;

        file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "could not open %s\n", argv[1]);
            exit(1);
        }
        yyrestart(file);
        yyparse();
    }
    else
    {    FILE *file;

        file = fopen("source.c", "r");
        if (!file) {
            fprintf(stderr, "could not open %s\n", argv[1]);
            exit(1);
        }
        yyrestart(file);
        yyparse();

    }
    return 0;
}

yyerror(char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}
void traverse(struct TreeNode* head, int depth)
{
    /*
    int i;
    struct TreeNode *sibling, *child;
    for(i = 0; i != 0; ++i)
        printf(" ");
    printf("Instant: %s(%d)\n",head->name,  head->lineno);

    child = head->firstChild;
    if(child != NULL){
        for(i = 0; i < 8000000; ++i)
        {
        }
        printf("firstChild's name%s, depth: %d\n", child->name, depth);
        traverse(child, depth+1);
    }
    
    sibling = head->nextSibling;
    while(sibling != NULL) {
        printf("sibling traverse");
        traverse(sibling, depth+1);
        sibling = sibling->nextSibling;
    }
    */
    int i;
    if(head == NULL)
        return ;
    printf("Instant: %s(%d)\n", head->name, head->lineno);
   // printf("FirstChild: %s\n", head->firstChild->name);
    traverse(head->firstChild, depth+1);
    traverse(head->nextSibling, depth+1);
}

void treeSet(struct TreeNode * s0, struct TreeNode * s1, struct TreeNode * s2, struct TreeNode * s3, struct TreeNode * s4)
{
    if( s1 == NULL){
        struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        temp->firstChild = NULL;
        temp->nextSibling = NULL;
        s0->ttn = temp;
        temp->name = s0->name;
        temp->lineno = s0->lineno;
        return ;
    }
    if(s1->reserveTag == 10) {
        struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        s1->ttn = temp;
        temp->nextSibling = NULL;
        temp->firstChild = NULL; 
        temp->name = s1->name;
        temp->lineno = s1->lineno;
    }
    struct TreeNode *temp0 = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    s0->ttn = temp0;
    temp0->firstChild = s1->ttn;
    temp0->nextSibling = NULL;
    if(s2 == NULL){
        return;
    }
    if(s2->reserveTag == 10) {
        struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        s2->ttn = temp;
        temp->nextSibling = NULL;
        temp->firstChild = NULL;
        temp->name = s2->name;
        temp->lineno = s2->lineno;
    }
    s1->ttn->nextSibling = s2->ttn;
    if( s3 == NULL)
        return ;
    if(s3->reserveTag == 10) {
        struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        s3->ttn = temp;
        temp->nextSibling = NULL;
        temp->firstChild = NULL;
        temp->name = s3->name;
        temp->lineno = s3->lineno;
    }
    s2->ttn->nextSibling = s3->ttn;
    if(s4 == NULL)
        return;
    if(s4->reserveTag == 10) {
        struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        s4->ttn = temp;
        temp->nextSibling = NULL;
        temp->firstChild = NULL;
        temp->name = s3->name;
        temp->lineno = s3->lineno;

    }
    s3->ttn->nextSibling = s3->ttn;
}



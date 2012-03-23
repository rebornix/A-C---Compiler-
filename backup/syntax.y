%{
#include<stdio.h>
struct TreeNode {
    int ival;
    double dval;
    char *value;
    char *token;
    struct TreeNode *firstChild;
    struct TreeNode *nextSibling;
    int lineno;
};
struct TreeNode *head;
void traverse(struct TreeNode *head, int depth);
struct TreeNode* bindSibling(struct TreeNode * left, struct TreeNode * right);
struct TreeNode* bindParent(struct TreeNode *parent, struct TreeNode *child);

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
%token <node>  INT 
%token <node>  FLOAT 
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

Program     :   ExtDefList  {   struct TreeNode *temp;
                                temp = bindSibling(&$1, NULL);
                                $$.token = "Program";
                                temp = bindParent(&$$, temp);
                                traverse(temp, 0);
                                }
            ;
ExtDefList  :   ExtDef  ExtDefList { struct TreeNode *temp; 
                                     temp = bindSibling(&$2, NULL);
                                     temp = bindSibling(&$1, temp);
                                     $$.token = "ExtDefList";
                                     bindParent(&$$, temp);} 
            |   /* empty */  {$$.token = NULL;}
            ;
ExtDef      :   Specifier   ExtDecList  SEMI   
            |   Specifier   SEMI 
            |   Specifier   FunDec  CompSt {struct TreeNode *temp; 
                                            temp = bindSibling(&$3, NULL);
                                            temp = bindSibling(&$2, temp);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "ExtDef";
                                            bindParent(&$$, temp);}
            ;
ExtDecList  :   VarDec
            |   VarDec  COMMA   ExtDecList
            ;

Specifier   :   TYPE   { struct TreeNode *temp;
                         temp = bindSibling(&$1, NULL); /* former: temp = bindSibling($$1, temp) ;*/
                         temp->firstChild = NULL;
                         $$.token = "Specifier"; 
                         bindParent(&$$, temp);}
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

VarDec      :   ID { struct TreeNode *temp;
                     temp = bindSibling(&$1, NULL);
                     $$.token = "VarDec"; 
                     bindParent(&$$, temp);}
            |   VarDec  LB  INT RB
            ;
FunDec      :   ID  LP  VarList RP
            |   ID  LP  RP  { struct TreeNode *temp;
                              temp = bindSibling(&$3, NULL);
                              temp = bindSibling(&$2, temp);
                              temp = bindSibling(&$1, temp);
                              $$.token = "FunDec";
                              bindParent(&$$, temp);}
            ;
VarList     :   ParamDec    COMMA   VarList
            |   ParamDec
            ;
ParamDec    :   Specifier   VarDec
            ;

CompSt      :   LC  DefList StmtList    RC {  struct TreeNode *temp;
                                             temp = bindSibling(&$4, NULL);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "CompSt";
                                             bindParent(&$$, temp); }
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

DefList     :   Def DefList {  struct TreeNode *temp;
                               temp = bindSibling(&$2, NULL);
                               temp = bindSibling(&$1, temp);
                               $$.token = "DefList";
                               bindParent(&$$, temp);}
            |
            ;
Def         :   Specifier   DecList SEMI {  struct TreeNode *temp;
                                            temp = bindSibling(&$3, NULL);
                                            temp = bindSibling(&$2, temp);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "Def";
                                            bindParent(&$$, temp); }
                                            ;
DecList     :   Dec {  struct TreeNode *temp;
                       temp = bindSibling(&$1, NULL);
                       $$.token = "DecList";
                       bindParent(&$$, temp); } 
            |   Dec COMMA DecList
            ;
Dec         :   VarDec  { struct TreeNode *temp;
                          temp = bindSibling(&$1, NULL);
                          $$.token = "VarDec"; 
                          bindParent(&$$, temp);}
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

    return 0;
}

yyerror(char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}
void traverse(struct TreeNode* head, int depth)
{
    struct TreeNode *child;
    int i;
    
    for(i = 0; i != depth; ++i)
        printf(" ");
    if(head->token != NULL){
        printf("%s(%d)\n", head->token, head->lineno);
        if(head->firstChild != NULL) {
             traverse(head->firstChild, depth+1);
        } 
        if(head->nextSibling == NULL){
        }
        else{
            traverse(head->nextSibling, depth);
        }
    }
    
}

struct TreeNode* bindSibling(struct TreeNode *left, struct TreeNode * right)
{
   struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode ));
   *temp = *left;
   temp->nextSibling = right;
   //printf("bind sibling");
   return temp;
}
struct TreeNode* bindParent(struct TreeNode *parent, struct TreeNode *child){
    parent->firstChild = child;
    return parent;
   // printf("bind parent ");
}

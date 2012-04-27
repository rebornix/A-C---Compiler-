%{

#include"stable.h"

int errorJudge = 0;
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
                                init_table();
                                if(errorJudge == 0){
                                    traverseInit(temp, 0);
                                    traverse(temp);
                                    }
                                trav_syn_table();
                                }
            ;
ExtDefList  :   ExtDef  ExtDefList { struct TreeNode *temp; 
                                     temp = bindSibling(&$2, NULL);
                                     temp = bindSibling(&$1, temp);
                                     $$.token = "ExtDefList";
                                     bindParent(&$$, temp);} 
            |   /* empty */  { $$.token = NULL; $$.nextSibling = NULL;  $$.firstChild = NULL; }
            ;
ExtDef      :   Specifier   ExtDecList  SEMI{struct TreeNode *temp; 
                                            temp = bindSibling(&$3, NULL);
                                            temp = bindSibling(&$2, temp);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "ExtDef";
                                            bindParent(&$$, temp);}   
            |   Specifier   SEMI {struct TreeNode *temp; 
                                            temp = bindSibling(&$2, NULL);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "ExtDef";
                                            bindParent(&$$, temp);}
            |   Specifier   FunDec  CompSt {
                                            struct TreeNode *temp; 
                                            temp = bindSibling(&$3, NULL);
                                            temp = bindSibling(&$2, temp);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "ExtDef";
                                            bindParent(&$$, temp);}
            ;
ExtDecList  :   VarDec{ struct TreeNode *temp;
                     temp = bindSibling(&$1, NULL);
                     $$.token = "ExtDecList"; 
                     bindParent(&$$, temp);}
            |   VarDec  COMMA   ExtDecList{struct TreeNode *temp; 
                                            temp = bindSibling(&$3, NULL);
                                            temp = bindSibling(&$2, temp);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "ExtDecList";
                                            bindParent(&$$, temp);}
            ;

Specifier   :   TYPE   { struct TreeNode *temp;
                         temp = bindSibling(&$1, NULL); /* former: temp = bindSibling($$1, temp) ;*/
                         temp->firstChild = NULL;
                         $$.token = "Specifier"; 
                         bindParent(&$$, temp);}
            |   StructSpecifier  { struct TreeNode *temp;
                     temp = bindSibling(&$1, NULL);
                     $$.token = "Specifier"; 
                     bindParent(&$$, temp);}
            ;
StructSpecifier :   STRUCT  OptTag  LC  DefList RC {struct TreeNode *temp; 
                                            temp = bindSibling(&$5, NULL);
                                            temp = bindSibling(&$4, temp);
                                            temp = bindSibling(&$3, temp);
                                            temp = bindSibling(&$2, temp);
                                            temp = bindSibling(&$1, temp);
                                            $$.token = "StructSpecifier";
                                            bindParent(&$$, temp);}
                |   STRUCT  Tag{ struct TreeNode *temp;
                            temp = bindSibling(&$2, NULL);
                            temp = bindSibling(&$1, temp);
                            $$.token = "StructSpecifier";
                            bindParent(&$$, temp);
                            } 
                ;
OptTag      :   ID { struct TreeNode *temp;
                     temp = bindSibling(&$1, NULL);
                     $$.token = "OptTag"; 
                     bindParent(&$$, temp);}
            |   /* empty */{ $$.token = NULL; $$.nextSibling = NULL;  $$.firstChild = NULL; }
            ;
Tag         :   ID{ struct TreeNode *temp;
                     temp = bindSibling(&$1, NULL);
                     $$.token = "Tag"; 
                     bindParent(&$$, temp);}
            ;

VarDec      :   ID { struct TreeNode *temp;
                     temp = bindSibling(&$1, NULL);
                     $$.token = "VarDec"; 
                     bindParent(&$$, temp);}
            |   VarDec  LB  INT RB{  struct TreeNode *temp;
                                             temp = bindSibling(&$4, NULL);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "VarDec";
                                             bindParent(&$$, temp); }
            ;
FunDec      :   ID  LP  VarList RP{  struct TreeNode *temp;
                                             temp = bindSibling(&$4, NULL);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "FunDec";
                                             bindParent(&$$, temp); }
            |   ID  LP  RP  { struct TreeNode *temp;
                              temp = bindSibling(&$3, NULL);
                              temp = bindSibling(&$2, temp);
                              temp = bindSibling(&$1, temp);
                              $$.token = "FunDec";
                              bindParent(&$$, temp);}
            ;
VarList     :   ParamDec    COMMA   VarList{ struct TreeNode *temp;
                              temp = bindSibling(&$3, NULL);
                              temp = bindSibling(&$2, temp);
                              temp = bindSibling(&$1, temp);
                              $$.token = "VarList";
                              bindParent(&$$, temp);}
            |   ParamDec{ struct TreeNode *temp;
                         temp = bindSibling(&$1, NULL);
                         $$.token = "VarList"; 
                         bindParent(&$$, temp);}
            ;
ParamDec    :   Specifier   VarDec{ struct TreeNode *temp;
                                    temp = bindSibling(&$2, NULL);
                                    temp = bindSibling(&$1, temp);
                                    $$.token = "ParamDec";
                                    bindParent(&$$, temp);}
            ;

CompSt      :   LC  DefList StmtList  RC { 
                                            struct TreeNode *temp;
                                             temp = bindSibling(&$4, NULL);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "CompSt";
                                             bindParent(&$$, temp); }
            
            ;
StmtList    :   Stmt    StmtList { struct TreeNode *temp;
                                    temp = bindSibling(&$2, NULL);
                                    temp = bindSibling(&$1, temp);// fuck: temp = bindSibling(&$1, &$1);
                                    $$.token = "StmtList";
                                    bindParent(&$$, temp);}
            |   { $$.token = NULL; $$.nextSibling = NULL;  $$.firstChild = NULL; }
            ;
Stmt        :   Exp SEMI { struct TreeNode *temp;
                            temp = bindSibling(&$2, NULL);
                            temp = bindSibling(&$1, temp);
                            $$.token = "Stmt";
                            bindParent(&$$, temp);
                            }
            |   CompSt{ struct TreeNode *temp;
                            temp = bindSibling(&$1, NULL);
                            $$.token = "Stmt";
                            bindParent(&$$, temp);
                            }
            |   RETURN  Exp SEMI{  struct TreeNode *temp;
                                             temp = bindSibling(&$3, NULL);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "Stmt";
                                             bindParent(&$$, temp); }
            |   IF  LP  Exp RP  Stmt  %prec LOWER_THAN_ELSE{  struct TreeNode *temp;
                                             temp = bindSibling(&$4, NULL);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "Stmt";
                                             bindParent(&$$, temp); }
            |   IF  LP  Exp RP  Stmt    ELSE    Stmt{  struct TreeNode *temp;
                                             temp = bindSibling(&$7, NULL);
                                             temp = bindSibling(&$6, temp);
                                             temp = bindSibling(&$5, temp);
                                             temp = bindSibling(&$4, temp);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "Stmt";
                                             bindParent(&$$, temp); }
            |   WHILE   LP  Exp RP  Stmt{ 
                                            printf("---------------------While-------------------- \n");
                                             struct TreeNode *temp;
                                             temp = bindSibling(&$5, NULL);
                                             temp = bindSibling(&$4, temp);
                                             temp = bindSibling(&$3, temp);
                                             temp = bindSibling(&$2, temp);
                                             temp = bindSibling(&$1, temp);
                                             $$.token = "Stmt";
                                             bindParent(&$$, temp); }
            ;
DefList     :   Def DefList {  struct TreeNode *temp;
                               temp = bindSibling(&$2, NULL);
                               temp = bindSibling(&$1, temp);
                               $$.token = "DefList";
                               bindParent(&$$, temp);}
            | { $$.token = NULL; $$.nextSibling = NULL;  $$.firstChild = NULL; } /* If I don't do anything, $$ will be initialized as $1 by default. */
                                                                                 /* This will case some strange bugs such as DefList reduce by       */
                                                                                 /* 'Def DecList' instead of 'Def DefList'.Maybe stack result in this*/ 
            ;
Def         :   Specifier DecList SEMI {  struct TreeNode *temp;
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
            |   Dec COMMA DecList{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "DecList";
                                        bindParent(&$$, temp); }
            ;
Dec         :   VarDec  { struct TreeNode *temp;
                          temp = bindSibling(&$1, NULL);
                          $$.token = "Dec"; 
                          bindParent(&$$, temp);}
            |   VarDec  ASSIGNOP    Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Dec";
                                        bindParent(&$$, temp); }
            ;

Exp         :   Exp ASSIGNOP  Exp {    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp AND Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp OR  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp RELOP  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp PLUS  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp MINUS  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp STAR  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp DIV  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   LP  Exp  RP{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   MINUS  Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$2, NULL);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   NOT   Exp{    struct TreeNode *temp;
                                        temp = bindSibling(&$2, NULL);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   ID   LP   Args  RP{    struct TreeNode *temp;
                                        temp = bindSibling(&$4, NULL);
                                        temp = bindSibling(&$3, temp);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   ID   LP   RP{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp   LB   Exp  RB{    struct TreeNode *temp;
                                        temp = bindSibling(&$4, NULL);
                                        temp = bindSibling(&$3, temp);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   Exp   DOT   ID{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp); }
            |   ID { struct TreeNode *temp;
                       temp = bindSibling(&$1, NULL);
                       $$.token = "Exp";
                       bindParent(&$$, temp); }
            |   INT { struct TreeNode *temp;
                       temp = bindSibling(&$1, NULL);
                       $$.token = "Exp";
                       bindParent(&$$, temp); }
            |   FLOAT{ struct TreeNode *temp;
                       temp = bindSibling(&$1, NULL);
                       $$.token = "Exp";
                       bindParent(&$$, temp); }
            ;
Args        :   Exp   COMMA   Args{    struct TreeNode *temp;
                                        temp = bindSibling(&$3, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Args";
                                        bindParent(&$$, temp); }
            |   Exp{ struct TreeNode *temp;
                       temp = bindSibling(&$1, NULL);
                       $$.token = "Args";
                       bindParent(&$$, temp); }
            ;

CompSt      :   error RC    { struct TreeNode *temp;
                                        temp = bindSibling(&$2, NULL);
                                        $$.token = "Compt";
                                        bindParent(&$$, temp);}
         ;
Exp     :   Exp LB error RB { struct TreeNode *temp;
                                        temp = bindSibling(&$4, NULL);
                                        temp = bindSibling(&$2, temp);
                                        temp = bindSibling(&$1, temp);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp);}
         |   error LP    { struct TreeNode *temp;
                                        temp = bindSibling(&$2, NULL);
                                        $$.token = "Exp";
                                        bindParent(&$$, temp);}
         ;
Stmt        :   error SEMI  { struct TreeNode *temp;
                                        temp = bindSibling(&$2, NULL);
                                        $$.token = "Stmt";
                                        bindParent(&$$, temp);
 }       

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


yyerror(char *msg)
{
    errorJudge = 1;
    fprintf(stderr, "Error type 2 at line %d:%s\n", yylineno, msg);
}

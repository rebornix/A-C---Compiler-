#include"stable.h"
#include<stdlib.h>
#include<string.h>
void init_table()
{
    syntax_table = NULL;
}

int tokenJudge(char *name){
    if(isupper(name[1]))  // if the second char is Upper, the name must be a token 
        return 1;
    else
        return 0;
}
void getType(char *type, Type structType, struct TreeNode* specifier)
{
    if( strcmp(specifier->firstChild->token, "TYPE") == 0 ){
        memcpy(type, specifier->firstChild->value, 32);
    }
    else { 
        /*
         * StructSpecifier
         */

    }
}

void addExtDecList(char* typeChar, struct TreeNode* ExtDecList)
{ 
    printf("addExtDecList entry\n"); // if '\0' is not added, the buffer will not be flushed ,then I will never know where the bug is.
    struct TreeNode* VarDec = ExtDecList->firstChild;
    if( VarDec->nextSibling != NULL){
        printf("##\n");
        addExtDecList(typeChar, VarDec->nextSibling->nextSibling);
    }

    SyntaxNode newNode = (SyntaxNode)malloc(sizeof(struct SyntaxNode_));
    newNode->name = malloc(32);
    memcpy(newNode->name, VarDec->firstChild->value+'\0', 32);
    newNode->kind = variable;
    newNode->next = NULL;

    if( strcmp(VarDec->firstChild->token, "ID" ) != 0 ){
        Type elemType = (Type)malloc(sizeof( struct Type_ ));
        if( strcmp(typeChar, "int")== 0  ){
            elemType->kind = basic;
            elemType->u.basic = 0;
        }
        else if( strcmp(typeChar, "float") == 1){
            elemType->kind = basic;
            elemType->u.basic = 1;
        }
        Type arrayType;
        struct TreeNode *temp = VarDec->firstChild;
        while( strcmp(temp->token, "ID") != 0 ){
            arrayType = (Type)malloc(sizeof( struct Type_ ));
            arrayType->kind = array;
            arrayType->u.array.elem = elemType;
            arrayType->u.array.size = temp->nextSibling->nextSibling->ival;
            elemType = arrayType;
            temp = temp->firstChild;
        }
        newNode->u.type = arrayType;
    }
    else{
        /*
         * set the type of the syntax
         */
        newNode->u.type = (Type)malloc(sizeof( struct Type_ ));
        if( strcmp(typeChar, "int") == 0  ){
            newNode->u.type->kind = basic;
            newNode->u.type->u.basic = 0;
        }
        if(  strcmp(typeChar, "float") == 0 )
        {   newNode->u.type->kind = basic;
            newNode->u.type->u.basic = 1;
        }
    }
    /*
     * to do list:  struct type
     */

    //add to the table
    if( syntax_table == NULL ) {
        syntax_table = newNode;
        printf("%s ..\n", syntax_table->name);
    }
    else{
        SyntaxNode itr = syntax_table;
        while( itr->next != NULL )
             itr = itr->next;
        itr->next = newNode;
    }
}
void addFunDec(char* returnType, struct TreeNode* FunDec)
{
    struct TreeNode* ID = FunDec->firstChild;
    SyntaxNode newNode = (SyntaxNode)malloc(sizeof(struct SyntaxNode_));
    newNode->name = malloc(32);
    memcpy(newNode->name, ID->value+'\0', 32);
    newNode->kind = function;
    newNode->next = NULL;

    newNode->u.func.returnType = (Type)malloc(sizeof( struct Type_ ));
    if( returnType == "int" ){
        newNode->u.func.returnType->kind = basic;
        newNode->u.func.returnType->u.basic = 0;
    }
    if( returnType == "float" ){
        newNode->u.func.returnType->kind = basic;
        newNode->u.func.returnType->u.basic = 1;
    }
    /*
     * to do : struct 
     */
    newNode->u.func.paramCount = 0;
    newNode->u.func.paramTypeList = NULL;
    printf("rp/varlist: %s\n", ID->nextSibling->nextSibling->value);
    if( strcmp(ID->nextSibling->nextSibling->value, ")") != 0 ){
    /*
     * param type list
     */
        printf("param exists\n");
        struct TreeNode * ParamDec = ID->nextSibling->nextSibling->firstChild;
        while(ParamDec != NULL ){
            struct TreeNode* Specifier = ParamDec->firstChild;
            char *typeList = malloc(32);
            newNode->u.func.paramCount++;
            getType(typeList, Specifier);
            if( newNode->u.func.paramTypeList == NULL ){
                newNode->u.func.paramTypeList = (FieldList)malloc(sizeof( struct FieldList_));
                newNode->u.func.paramTypeList->name = malloc(32);
                memcpy(newNode->u.func.paramTypeList->name, typeList+'\0', 32);
                newNode->u.func.paramTypeList->tail = NULL;
                newNode->u.func.paramTypeList->type = (Type)malloc(sizeof( struct Type_ ));
                if( strcmp(typeList, "int") == 0 ){
                    newNode->u.func.paramTypeList->type->kind = basic;
                    newNode->u.func.paramTypeList->type->u.basic = 0;
                }
                else if( strcmp( typeList, "float") == 0 ){
                    newNode->u.func.paramTypeList->type->kind = basic;
                    newNode->u.func.paramTypeList->type->u.basic = 1;
                }
                /*
                 * struct undone
                 */
            }
            else{
                FieldList param = newNode->u.func.paramTypeList;
                while( param->tail != NULL)
                    param = param->tail;
                FieldList newParam = (FieldList) malloc( sizeof ( struct FieldList_ ));
                newParam->name = malloc(32);
                memcpy(newParam->name, typeList+'\0', 32);
                newParam->tail = NULL;
                newParam->type = (Type)malloc(sizeof( struct Type_ ));
                if( strcmp(typeList, "int") == 0 ){
                    newParam->type->kind = basic;
                    newParam->type->u.basic = 0;
                }
                else if( strcmp( typeList, "float") == 0 ){
                    newParam->type->kind = basic;
                    newParam->type->u.basic = 1;
                }
                param->tail = newParam;
            }
            if( ParamDec->nextSibling != NULL){
                ParamDec = ParamDec->nextSibling->nextSibling->firstChild;
            }
            else
                ParamDec = NULL;
        }
    }
    printf("after param\n");
    if( syntax_table == NULL ) {
        syntax_table = newNode;
        printf("%s ..\n", syntax_table->name);
    }
    else{
        SyntaxNode itr = syntax_table;
        while( itr->next != NULL )
             itr = itr->next;
        itr->next = newNode;
    }

}
void traverseInit(struct TreeNode* head, int depth)
{
    struct TreeNode *child;
    int i;

    if(head->token != NULL){
        for(i = 0; i != depth; ++i)
            printf("  ");
        if(tokenJudge(head->token)){
            if( head->token == "INT")
                printf("%s:%d\n", head->token, head->ival);
            else if (head->token == "FLOAT")
                printf("%s:%.6f\n", head->token, head->dval);
            else if (head->token == "ID" )
                printf("%s: %s\n", head->token, head->value);
            else
                printf("%s\n", head->token);
        }
        else
            printf("%s(%d)\n", head->token, head->lineno);

        if(head->firstChild != NULL) {
            traverseInit(head->firstChild, depth+1);
        } 
        if(head->nextSibling == NULL){
        }
        else{
            traverseInit(head->nextSibling, depth);
        }
    }

}
void traverse(struct TreeNode* head)
{
    struct TreeNode *child;
    int i;
    
     if(head->token != NULL){
        //Add to Syntax table
        if( strcmp(head->token,"ExtDef") == 0 ) { //ExtDef : Specifier

            /*
             * get type from Specifier
             */
            struct TreeNode* specifier = head->firstChild;
            char *type = malloc(32);
            printf("get Type !!! \n");
            getType(type, specifier);
            printf("............... %s .............\n", type);
            /*
             * get Varlist or func list
             */
            struct TreeNode* secondChild = specifier->nextSibling;
            if( strcmp(secondChild->token, "ExtDecList") == 0 ){
                addExtDecList(type, secondChild);
            }
            else if( strcmp(secondChild->token, "FunDec")== 0 ){
               addFunDec(type, secondChild);
            }
        }
        if( strcmp(head->token, "Def") == 0){ // Def: Specifier DecList SEMI
            printf(" get Def \n");
        }

        /*
         * Traverse the syntax table to look for the variable or function
         */
        if( strcmp(head->token, "Exp") == 0 ){
        }

        /*
         * recursive
         */
        if(head->firstChild != NULL) {
             traverse(head->firstChild);
        } 
        if(head->nextSibling == NULL){
        }
        else{
            traverse(head->nextSibling);
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

void trav_syn_table(){
    SyntaxNode itr = syntax_table;
    printf(" traverse the syntax table\n");
    while( itr != NULL ){
        printf("%s", itr->name);
        if( itr->kind == function ){
            printf("count: %d\n", itr->u.func.paramCount);
            FieldList newList = itr->u.func.paramTypeList;
            while(newList!=NULL){
                printf("type %s\n", newList->name);
                newList = newList->tail;
            }
        }
        else{
            if( itr->u.type->kind == basic ){
                printf(" basic: %d\n", itr->u.type->u.basic);
            }
            else if( itr->u.type->kind == array ){
                printf(" array: size[%d][%d]\n", itr->u.type->u.array.size, itr->u.type->u.array.elem->u.array.size);
                
            }
        }
        itr = itr->next;
    }
}

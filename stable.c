#include"stable.h"
#include<stdlib.h>
#include<string.h>
//#define DEBUG
#define COMPILE
void init_table()
{
    syntax_table = NULL;
}

void getType(Type type,  struct TreeNode* specifier)
{
#ifdef DEBUG
    printf("getType %s \n", specifier->firstChild->token);
#endif
    if( strcmp(specifier->firstChild->token, "TYPE") == 0 ){
        if( strcmp( specifier->firstChild->value, "int") == 0){
            type->kind = basic;
            type->u.basic = 0;
        }
        if( strcmp( specifier->firstChild->value, "float") == 0){
            type->kind = basic;
            type->u.basic = 1;
        }
    }
    else { 
        /*
         * StructSpecifier
         */
#ifdef DEBUG
        printf("enter StructSpecifier\n");
#endif
        type->kind = structure;
        struct TreeNode* TAG = specifier->firstChild->firstChild->nextSibling;
        type->u.structure = (FieldList)malloc(sizeof(struct FieldList_));
        if( TAG->token != NULL &&  strcmp( TAG->token, "Tag") == 0 ){
            if( findUserType(TAG->firstChild->value) == 0 ){
#ifdef COMPILE
                printf("Error Type 17 at line %d : Undefined type: %s\n", TAG->lineno, TAG->firstChild->value);
#endif
            }
            else {
                type->u.structure->name = malloc(32);
                memcpy(type->u.structure->name, TAG->firstChild->value, 32);
                type->u.structure->type = NULL;
                type->u.structure->tail = NULL;
            }
        }
        else{
            if( TAG->token != NULL ){ // add to the user_type_list or output an erro message if it exites already.
                type->u.structure->name = malloc(32);
                memcpy(type->u.structure->name, TAG->firstChild->value, 32);
            }
            else
                type->u.structure->name = NULL; //undefined
            type->u.structure->tail = NULL;
#ifdef DEBUG
            printf("structure start from here\n");
#endif
            struct TreeNode* DefList = TAG->nextSibling->nextSibling;
            while(DefList != NULL){
                struct TreeNode* Def = DefList->firstChild;
                Type type_temp = (Type)malloc(sizeof(struct Type_));
                getType(type_temp, Def->firstChild);
#ifdef DEBUG
                printf("get type recursively\n");
#endif
                struct TreeNode* DecList = Def->firstChild->nextSibling;

                do {
                    struct TreeNode* VarDec = DecList->firstChild->firstChild;
                    FieldList newField = (FieldList)malloc(sizeof(struct FieldList_));
                    newField->type = (Type)malloc(sizeof(struct Type_));
                    /*
                     * VarDec Assignop Exp undone
                     */
                    if( strcmp(VarDec->firstChild->token, "ID" ) != 0 ){
                        Type elemType = (Type)malloc(sizeof( struct Type_ ));
                        elemType->kind = type_temp->kind;
                        if( type_temp->kind == basic )
                            elemType->u.basic = type_temp->u.basic;
                        
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
                        newField->type = arrayType;
                        newField->tail = NULL;
                    }
                    else{
                        /*
                         * set the type of the syntax
                         */
                        newField->tail = NULL;
                        newField->type->kind = type_temp->kind;
                        if( type_temp->kind == basic)
                            newField->type->u.basic = type_temp->u.basic;
                    }
                    /*
                     * add to fieldlist
                     */
                    FieldList itr = type->u.structure;
                    while(itr->tail != NULL)
                        itr = itr->tail;
                    itr->tail = newField;
                    if( DecList->firstChild->nextSibling == NULL)
                        DecList = NULL;
                    else
                        DecList = DecList->firstChild->nextSibling->nextSibling;
                }
                while( DecList != NULL );

                if( DefList->firstChild->nextSibling->token  == NULL) // nextsibling is not NULL , in my implementation
                    DefList = NULL;
                else
                DefList = DefList->firstChild->nextSibling;
            }
#ifdef DEBUG
            printf("-----------------struct name: %s--------------------------\n", type->u.structure->name);
#endif
            if( user_type_table == NULL ){
                user_type_table = (UserType)malloc(sizeof(struct UserType_));
                user_type_table->name = malloc(32);
                memcpy(user_type_table->name, type->u.structure->name, 32);
                user_type_table->tail = type->u.structure->tail;
                user_type_table->next = NULL;
            }
            else{
                UserType itr = user_type_table;
                int reDefError = 0;
                if( findUserType(type->u.structure->name) == 1 )
                    reDefError = 1;
                if( reDefError == 0 ){
                    itr->next = (UserType)malloc(sizeof(struct UserType_));
                    itr->next->name = malloc(32);
                    memcpy(itr->next->name, type->u.structure->name, 32);
                    itr->next->tail = type->u.structure->tail;
                    itr->next->next = NULL;
                }
                else{
#ifdef COMPILE
                    printf("Error Type 16 at line %d : Redefined Structure %s\n",TAG->lineno,  type->u.structure->name);
#endif
                }
            }
        }
    }
}

int lValueJudge(struct TreeNode* Exp)
{
    struct TreeNode* firstChild = Exp->firstChild;
    if( strcmp( firstChild->token ,"ID" ) == 0 )
    {
        if( findFunc(firstChild->value) == 1 )
            return 0;
        else
            return 1;
    }
    if( strcmp( firstChild->token, "INT" ) == 0 || strcmp( firstChild->token, "FLOAT" ) == 0 )
        return 0;
    if( strcmp( firstChild->token, "LP" ) == 0 || strcmp( firstChild->token, "MINUS" ) == 0 || strcmp( firstChild->token, "NOT") == 0)
        return lValueJudge(firstChild->nextSibling);
    // firstChild must be exp
    if( firstChild->nextSibling != NULL && strcmp( firstChild->nextSibling->token, "ASSIGNOP") == 0) {
        if( lValueJudge(firstChild->nextSibling->nextSibling) == 0)
            return 0;
        if( lValueJudge(firstChild) == 0 )
            return 0;
        }
    return 1;
}

void addVarDec(Type type, struct TreeNode* VarDec)
{
#ifdef DEBUG
    printf("add dec vardec-firstChild- value : %s\n", VarDec->firstChild->value);
#endif
    if( findName(VarDec->firstChild->value) == 1 ){
#ifdef COMPILE
        printf("Error Type 3 at line %d : Variable Redefined\n",VarDec->lineno,  VarDec->firstChild->value);
#endif
        return ;
    }
    SyntaxNode newNode = (SyntaxNode)malloc(sizeof(struct SyntaxNode_));
    newNode->name = malloc(32);
    memcpy(newNode->name, VarDec->firstChild->value+'\0', 32);
    newNode->kind = variable;
    newNode->next = NULL;

    if( strcmp(VarDec->firstChild->token, "ID" ) != 0 ){
        Type elemType = (Type)malloc(sizeof( struct Type_ ));
        elemType->kind = type->kind;
        if( type->kind == basic )
            elemType->u.basic = type->u.basic;

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
#ifdef DEBUG
        printf(" ********************** x **********************\n");
#endif
        newNode->u.type = (Type)malloc(sizeof( struct Type_ ));
        newNode->u.type->kind = type->kind;
        if( type->kind == basic){
            newNode->u.type->u.basic = type->u.basic;
        }
        else if ( type->kind == array ){
            newNode->u.type->u.array = type->u.array;
        }
        else {
            newNode->u.type->u.structure = type->u.structure;
        }
    }
    /*
     * to do list:  struct type
     */

    //add to the table
    if( syntax_table == NULL ) {
        syntax_table = newNode;
    }
    else{
        SyntaxNode itr = syntax_table;
        while( itr->next != NULL )
             itr = itr->next;
        itr->next = newNode;
    }
}
void addExtDecList(Type type, struct TreeNode* ExtDecList)
{
#ifdef DEBUG
    printf("addExtDecList entry\n"); 
#endif
    struct TreeNode* VarDec = ExtDecList->firstChild;
    if( VarDec->nextSibling != NULL){
        Type newType = (Type)malloc(sizeof(struct Type_));
        memcpy(newType, type, sizeof(struct Type_));
        addExtDecList(newType, VarDec->nextSibling->nextSibling);
    }
    addVarDec(type, VarDec);
}
void addDec(Type type, struct TreeNode* DecList)
{
#ifdef DEBUG
    printf("addDec entry\n");
    printf("dec type: %d\n", type->kind);
#endif
    struct TreeNode* Dec = DecList->firstChild;
    if( Dec->nextSibling != NULL ){
        Type newType = (Type)malloc(sizeof(struct Type_));
        memcpy(newType, type, sizeof(struct Type_));
        addDec(newType, Dec->nextSibling->nextSibling);
    }
    struct TreeNode* VarDec = Dec->firstChild;
    addVarDec(type, VarDec);
}
void addFunDec(Type returnType, struct TreeNode* FunDec)
{
    if ( findName(FunDec->firstChild->value) == 1 ){
#ifdef COMPILE
        printf("Error Type 4 at line %d: Function Redefined \n", FunDec->lineno);
#endif 
        return ;
    }
    struct TreeNode* ID = FunDec->firstChild;
    SyntaxNode newNode = (SyntaxNode)malloc(sizeof(struct SyntaxNode_));
    newNode->name = malloc(32);
    memcpy(newNode->name, ID->value+'\0', 32);
    newNode->kind = function;
    newNode->next = NULL;

    newNode->u.func.returnType = (Type)malloc(sizeof( struct Type_ ));
    newNode->u.func.returnType->kind = returnType->kind;
    if( returnType->kind == basic )
        newNode->u.func.returnType->u.basic = returnType->u.basic;

    /*
     * to do : struct 
     */
    newNode->u.func.paramCount = 0;
    newNode->u.func.paramTypeList = NULL;
    if( strcmp(ID->nextSibling->nextSibling->value, ")") != 0 ){
    /*
     * param type list
     */
#ifdef DEBUG
        printf("param exists\n");
#endif
        struct TreeNode * ParamDec = ID->nextSibling->nextSibling->firstChild;
        while(ParamDec != NULL ){
            struct TreeNode* Specifier = ParamDec->firstChild;
            struct TreeNode* VarDec = Specifier->nextSibling;
            //char *typeList = malloc(32);
            Type typeList = (Type)malloc(sizeof(struct Type_));
            newNode->u.func.paramCount++;
            getType(typeList, Specifier);
            addVarDec(typeList, VarDec);
            while( strcmp(VarDec->token, "ID") == 0 )
                VarDec = VarDec->firstChild;
            char *idName = malloc(32);
            memcpy(idName, VarDec->value, 32);
            if( newNode->u.func.paramTypeList == NULL ){
                newNode->u.func.paramTypeList = (FieldList)malloc(sizeof( struct FieldList_));
                newNode->u.func.paramTypeList->name = malloc(32);
                memcpy(newNode->u.func.paramTypeList->name, idName, 32);
                newNode->u.func.paramTypeList->tail = NULL;
                newNode->u.func.paramTypeList->type = (Type)malloc(sizeof( struct Type_ ));

                newNode->u.func.paramTypeList->type->kind = typeList->kind;
                if(typeList->kind == basic)
                   newNode->u.func.paramTypeList->type->u.basic = typeList->u.basic;
                if(typeList->kind == array ){
                    newNode->u.func.paramTypeList->type->u.array.size = typeList->u.array.size;
                    newNode->u.func.paramTypeList->type->u.array.elem = typeList->u.array.elem;
                }
                if(typeList->kind == structure){
                    newNode->u.func.paramTypeList->type->u.structure = typeList->u.structure;
                }
            }
            else{
                FieldList param = newNode->u.func.paramTypeList;
                while( param->tail != NULL)
                    param = param->tail;
                FieldList newParam = (FieldList) malloc( sizeof ( struct FieldList_ ));
                newParam->name = malloc(32);
                memcpy(newParam->name, idName, 32);
                newParam->tail = NULL;
                newParam->type = (Type)malloc(sizeof( struct Type_ ));
                newParam->type->kind = typeList->kind; 
                if( typeList->kind == basic )
                    newParam->type->u.basic = typeList->u.basic;
                param->tail = newParam;
            }
            if( ParamDec->nextSibling != NULL){
                ParamDec = ParamDec->nextSibling->nextSibling->firstChild;
            }
            else
                ParamDec = NULL;
        }
    }
    if( syntax_table == NULL ) {
        syntax_table = newNode;
    }
    else{
        SyntaxNode itr = syntax_table;
        while( itr->next != NULL )
             itr = itr->next;
        itr->next = newNode;
    }


}
void expFunc(struct TreeNode* Exp)
{
#ifdef DEBUG
    printf("expFunc entry  token: %s\n", Exp->token);
#endif
    struct TreeNode* firstChild = Exp->firstChild;
    if( strcmp( firstChild->token, "Exp" ) == 0) { // the firstChild is an Exp 
        expFunc( firstChild );
        if( strcmp(firstChild->nextSibling->token, "DOT") == 0 ){
#ifdef DEBUG
            printf("Exp DOT ID \n");
#endif
            // check whether the id existed in struct
        }
        else{
#ifdef DEBUG
            printf("Exp ... Exp\n");
#endif
            expFunc( firstChild->nextSibling->nextSibling );
        }

    }
    if( strcmp( firstChild->token, "ID" ) == 0){
#ifdef DEBUG
        printf("ID \n");
#endif
        struct TreeNode* ID = firstChild;
        if( findName(ID->value) ==  0){
            if( ID->nextSibling == NULL ){
                printf("Error Type 1 at line %d: Undefined variable \"%s\"\n",ID->lineno, ID->value);
            }
            else
                printf("Error Type 1 at line %d: Undefined function \"%s\"\n",ID->lineno, ID->value);
        }
        else {
            if( ID->nextSibling != NULL ){
                // function 
                if( findFunc(ID->value) == 0 )
                    printf("Error Type 11 at line %d : Variable can't be called as a funtion\n", ID->lineno);
            }
        }
    }
    if( strcmp ( firstChild->token, "LP") == 0 || strcmp(firstChild->token, "MINUS")== 0 || strcmp(firstChild->token, "NOT") == 0){
#ifdef DEBUG
        printf("LP/MINUS/NOT Exp\n");
#endif
        expFunc( firstChild->nextSibling );
    }
}



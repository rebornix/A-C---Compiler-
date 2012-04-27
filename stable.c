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
int checkTypeEqual(Type firstType, Type thirdType)
{
    if( firstType->kind != thirdType->kind )
        return 0;
    if( firstType->kind == basic ){
        if( firstType->u.basic == thirdType->u.basic )
            return 1;
        else
            return 0;
    }
    if( firstType->kind == array ){
        if ( firstType->u.array.size != thirdType->u.array.size )
            return 0;
        else
            return checkTypeEqual(firstType->u.array.elem, thirdType->u.array.elem);
    }
    if( firstType->kind == structure ){
        if( strcmp(firstType->u.structure->name, thirdType->u.structure->name) == 0 )
            return 1;
        else
            return 0;
    }
    return 1;
}

void getExpType(Type sType, struct TreeNode* currentNode)
{
#ifdef DEBUG
    printf("getExpType entry\n");
#endif
   if( strcmp( currentNode->token, "INT" ) == 0 ){
       sType->kind = basic;
       sType->u.basic = 0;
       return ;
   }
   if( strcmp( currentNode->token, "FLOAT" ) == 0 ){
       sType->kind = basic;
       sType->u.basic = 1;
       return ;
   }
   if( strcmp( currentNode->token, "ID" ) == 0 && currentNode->nextSibling == NULL ){
        SyntaxNode itr = syntax_table;
        while( itr != NULL ){
            if( strcmp( itr->name, currentNode->value) == 0 )
                break;
            itr = itr->next;
        }
        if( itr == NULL ) {
            return ;
        }
        if( itr->kind == variable ){
            //sType = itr->u.type;  Fuck ! U can't reset the pointer !!! 
            if( itr->u.type->kind == basic ){
                sType->kind = basic;
                sType->u.basic = itr->u.type->u.basic;
            }
            else if( itr->u.type->kind == array ){
                sType->kind = array;
                sType->u.array = itr->u.type->u.array;
            }
            else {
                sType->kind = structure;
                sType->u.structure = itr->u.type->u.structure;
            }
        }
        else{
         //   sType = itr->u.func.returnType;
            sType->kind = itr->u.func.returnType->kind;
            if( itr->u.func.returnType->kind == basic ) {
                sType->kind = basic;
                sType->u.basic = itr->u.func.returnType->u.basic;
            }
            else if ( itr->u.func.returnType->kind == array ) {
                sType->kind = array;
                sType->u.array = itr->u.func.returnType->u.array;
            }
            else{
                sType->kind = structure;
                sType->u.structure = itr->u.func.returnType->u.structure;
            }
        }
        return ;
   }
   if( strcmp( currentNode->token, "ID" ) == 0 && currentNode->nextSibling != NULL ){
        if( findFunc(currentNode->value) == 0 )
            return ; // check already done in expFunc, such a silly implementation
        else{
            SyntaxNode itr = syntax_table;
            while( itr != NULL ){
                if( strcmp(itr->name, currentNode->value) == 0 )
                    break;
                itr = itr->next;
            }
            if( itr->u.func.paramTypeList == NULL && strcmp(currentNode->nextSibling->nextSibling->token, "RP") == 0)
                return ;
            if( itr->u.func.paramTypeList == NULL && strcmp(currentNode->nextSibling->nextSibling->token, "RP") != 0){
                printf("Error Type 9 at line %d: param number mismatched\n", currentNode->lineno);
                return;
            }
            if( itr->u.func.paramTypeList != NULL && strcmp(currentNode->nextSibling->nextSibling->token, "RP") == 0){
                printf("Error Type 9 at line %d: param number mismatched\n", currentNode->lineno);
                return ;
            }
            if( checkParamType(itr->u.func.paramTypeList, currentNode->nextSibling->nextSibling) == 0 ){
                printf("Error Type 9 at line %d: param number mismatched\n", currentNode->lineno);
                return ;
            }
        }

   }
   if( strcmp( currentNode->token, "LP") == 0 || strcmp( currentNode->token, "MINUS") == 0 || strcmp( currentNode->token, "NOT") == 0 ){
       getExpType(sType, currentNode->nextSibling);
       return ;
   }
   if( strcmp( currentNode->token, "Exp" ) == 0 ){
       if( currentNode->nextSibling != NULL && strcmp(currentNode->nextSibling->token, "ASSIGNOP") == 0 ){
           if( strcmp(currentNode->firstChild->token, "ID") == 0 ){
               if( findFunc(currentNode->firstChild->value) == 1){
                  printf("Error Type 6 at line %d: Rvalue can be Lvalue\n", currentNode->lineno);
                  return ;
               }
               if( strcmp(currentNode->firstChild->token,"INT") == 0 || strcmp(currentNode->firstChild->token,"FLOAT") == 0 ){
                  printf("Error Type 6 at line %d: Rvalue can be Lvalue\n", currentNode->lineno);
                  return ;
               }
           }
           Type firstType = (Type)malloc(sizeof(struct Type_));
           Type thirdType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->firstChild);
           getExpType(thirdType, currentNode->nextSibling->nextSibling->firstChild);
           if( checkTypeEqual(firstType, thirdType) == 0 ){
               printf("Error Type 5 at line %d : Type mismatched\n", currentNode->lineno);
           }
           return ;
       }
       if( currentNode->nextSibling != NULL && strcmp(currentNode->nextSibling->token, "DOT") == 0 ){
       }
       if( currentNode->nextSibling != NULL && ( strcmp(currentNode->nextSibling->token, "AND") == 0 || 
                   strcmp(currentNode->nextSibling->token, "OR") == 0 ||
                   strcmp(currentNode->nextSibling->token, "RELOP") == 0 ||
                   strcmp(currentNode->nextSibling->token, "PLUS") == 0 ||
                   strcmp(currentNode->nextSibling->token, "MINUS") == 0 ||
                   strcmp(currentNode->nextSibling->token, "STAR") == 0 ||
                   strcmp(currentNode->nextSibling->token, "DIV") == 0 ))
       {
           Type firstType = (Type)malloc(sizeof(struct Type_));
           Type thirdType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->firstChild);
           getExpType(thirdType, currentNode->nextSibling->nextSibling->firstChild);
           if( checkTypeEqual(firstType, thirdType) == 0 ){
               printf("Error Type 7 at line %d \n", currentNode->lineno);
           }
           return ;
       }
       if( currentNode->nextSibling != NULL && (strcmp(currentNode->nextSibling->token, "LB") == 0)){
           Type firstType = (Type)malloc(sizeof(struct Type_));
           Type thirdType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->firstChild);
           getExpType(thirdType, currentNode->nextSibling->nextSibling->firstChild);
           if( firstType->kind != array ){
               printf("Error Type 10 at line %d\n", currentNode->lineno);
               sType->kind = firstType->kind;
               if(sType->kind == basic )
                   sType->u.basic = firstType->u.basic;
               else
                   sType->u.structure = sType->u.structure;
               return ;
           }
           else{
              if(thirdType->kind != basic || thirdType->u.basic != 0 ){
                  printf("Error Type 12 at line %d \n", currentNode->lineno);
                  sType->kind  = firstType->u.array.elem->kind;
                  if( sType->kind == basic )
                      sType->u.basic = firstType->u.array.elem->u.basic;
                  else if ( sType->kind == array )
                      sType->u.array = firstType->u.array.elem->u.array;
                  else
                      sType->u.structure = firstType->u.array.elem->u.structure;
              }
              else {
                  sType->kind  = firstType->u.array.elem->kind;
                  if( sType->kind == basic )
                      sType->u.basic = firstType->u.array.elem->u.basic;
                  else if ( sType->kind == array )
                      sType->u.array = firstType->u.array.elem->u.array;
                  else
                      sType->u.structure = firstType->u.array.elem->u.structure;
              }
           }
           return ;
       }
       getExpType(sType, currentNode->firstChild);
       return ;
   }
   return ;
}
int checkParamType(FieldList paramTypeList, struct TreeNode* Args)
{
    FieldList pitr = paramTypeList;
    struct TreeNode* titr = Args;
    if( pitr == NULL && titr == NULL )
        return 1;
    if( pitr != NULL && titr != NULL ){
        Type param = pitr->type;

        Type expType = (Type)malloc(sizeof(struct Type_));
        getExpType(expType, titr->firstChild);
        if( checkTypeEqual(param, expType) == 1 ){
            pitr = pitr->tail;
            if( titr->firstChild->nextSibling == NULL )
                titr = NULL;
            else
                titr = titr->firstChild->nextSibling->nextSibling;
            return checkParamType(pitr, titr);
        }
        else {
            return 0;
        }
    }
    return 0;
}
void returnTypeJudge(struct TreeNode* Exp)
{
    Type type = (Type)malloc(sizeof(struct Type_));
    getExpType(type, Exp);
    SyntaxNode itr = syntax_table;
    SyntaxNode func = syntax_table;
    while( itr != NULL ){
        if( itr->kind == function )
            func = itr;
        itr = itr->next;
    }
    Type returnType = func->u.func.returnType;
    if( checkTypeEqual(type, returnType) == 0 )
        printf("Error Type 8 at line %d: return type mismatch\n", Exp->lineno);
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
            /*
             * type check
             */
            
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
void traverseInit(struct TreeNode* head, int depth)
{
#ifdef DEBUG
    struct TreeNode *child;
    int i;

    if(head->token != NULL){
        for(i = 0; i != depth; ++i)
            printf("  ");
        if(tokenJudge(head->token)){
            if( strcmp(head->token, "INT") == 0)
                printf("%s:%d\n", head->token, head->ival);
            else if ( strcmp( head->token , "FLOAT") == 0)
                printf("%s:%.6f\n", head->token, head->dval);
            else if (strcmp(head->token, "ID") == 0 )
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
    else{
        if( head->nextSibling != NULL)
            traverseInit(head->nextSibling, depth);
    }
#endif
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
            Type type = (Type)malloc(sizeof(struct Type_));
            getType(type, specifier);
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
            struct TreeNode* specifier = head->firstChild;
            Type type = (Type)malloc(sizeof(struct Type_));
            getType(type, specifier);
            struct TreeNode* DecList = specifier->nextSibling;
            addDec(type, DecList);
        }

        if( strcmp(head->token, "RETURN") == 0 ){
            /*
             * check the return type
             */
            returnTypeJudge(head->nextSibling);
        }
        /*
         * Traverse the syntax table to look for the variable or function
         */
        if( strcmp(head->token, "Exp") == 0 ){
            expFunc(head);
            Type sType = (Type)malloc(sizeof(struct Type_));
            getExpType(sType, head);
            if( head->nextSibling != NULL && strcmp(head->nextSibling->token, "RP") == 0){
                if( head->nextSibling->nextSibling != NULL && strcmp( head->nextSibling->nextSibling->token,"Stmt") == 0)
                    traverse(head->nextSibling->nextSibling);
            }
            return; // if not return , the child Exp will be checked several times
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
     else{
        if( head->nextSibling != NULL )
            traverse(head->nextSibling);
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
#ifdef DEBUG
    SyntaxNode itr = syntax_table;
    printf("######   traverse the syntax table   ######\n");
    while( itr != NULL ){
        printf("%s: ", itr->name);
        if( itr->kind == function ){
            printf("count: %d\n", itr->u.func.paramCount);
            FieldList newList = itr->u.func.paramTypeList;
            while(newList!=NULL){
                printf("type %s\n", newList->name);
                newList = newList->tail;
            }
        }
        else if (itr->kind == variable ){
            if( itr->u.type->kind == basic ){
                printf(" basic: %d\n", itr->u.type->u.basic);
            }
            else if( itr->u.type->kind == array ){
                printf(" array: size[%d][%d]\n", itr->u.type->u.array.size, itr->u.type->u.array.elem->u.array.size);
                
            }
            else{
                printf(" %s ", itr->u.type->u.structure->name);
                printf("structure\n");
            }
        }
        itr = itr->next;
    }
#endif
}

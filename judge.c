#include"stable.h"
#include<stdlib.h>
#include<string.h>

int tokenJudge(char *name){
    if(isupper(name[1]))  // if the second char is Upper, the name must be a token 
        return 1;
    else
        return 0;
}
int findUserType(char *typeName)
{
    UserType itr = user_type_table;
    int found = 0;
    while( itr != NULL ){ 
        if( strcmp(itr->name, typeName) == 0 ){
            found = 1;
            break;
        }
        itr = itr->next;
    }
    return found;
}
int findName(char *name){
    SyntaxNode itr = syntax_table;
    int found = 0;
    while( itr != NULL ){
        if( strcmp(itr->name,name) == 0  ){
            found = 1;
            break;
        }
        itr = itr->next;
    }
    return found;
}
int findVariable(char *varName)
{
    SyntaxNode itr = syntax_table;
    int found = 0;
    while( itr != NULL ){
        if( strcmp(itr->name, varName) == 0 && itr->kind == variable ){
            found = 1;
            break;
        }
        itr = itr->next;
    }
    return found;
}
int findFunc(char *funcName)
{
    SyntaxNode itr = syntax_table;
    int found = 0;
    while( itr != NULL ){
        if( strcmp(itr->name, funcName) == 0 && itr->kind == function ){
            found = 1;
            break;
        }
        itr = itr->next;
    }
    return found;
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
           /*
           * Lvalue
           */
           if( lValueJudge(currentNode) == 0 ){
                  printf("Error Type 6 at line %d: Rvalue can be Lvalue\n", currentNode->lineno);
                  return ;
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
           Type firstType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->firstChild);
           if( firstType->kind != structure )
               printf("Error Type 13 at line %d\n", currentNode->lineno);
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
        /*
        if ( firstType->u.array.size != thirdType->u.array.size )
            return 0;
        else*/  // for C language 
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



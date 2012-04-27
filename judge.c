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

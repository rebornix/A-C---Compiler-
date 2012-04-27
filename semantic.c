#include"stable.h"
#include<stdlib.h>
#include<string.h>
//#define DEBUG
#define COMPILE
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

#include"stable.h"
#include<stdlib.h>
#include<string.h>
//#define DEBUG
#define COMPILE

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

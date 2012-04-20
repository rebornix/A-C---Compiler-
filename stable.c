#include"stable.h"

void addToTable()
{
    syntax_table = (SyntaxNode)malloc(sizeof(struct SyntaxNode_));
    syntax_table->name = "test";
    syntax_table->u.type = (Type)malloc(sizeof(struct Type_));
    syntax_table->u.type->kind = basic;
    syntax_table->u.type->u.basic = 1;
}

int tokenJudge(char *name){
    if(isupper(name[1]))  // if the second char is Upper, the name must be a token 
        return 1;
    else
        return 0;
}
void traverse(struct TreeNode* head, int depth)
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

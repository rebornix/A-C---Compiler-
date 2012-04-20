#include<stdio.h>
#include<ctype.h>

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


typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SyntaxNode_* SyntaxNode;
struct Type_
{
    enum { basic, array, structure } kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        FieldList structure;
    } u;
};

struct FieldList_
{
    char * name;
    Type type;
    FieldList tail;
};
struct SyntaxNode_
{
    char *name;
    union
    {
        struct { Type returnType;
                 Type paramTypeList;
                 int paramCount;
        } func;
        Type type;
    } u;
};

SyntaxNode syntax_table;


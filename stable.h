#include<stdio.h>
#include<ctype.h>
#include<stdio.h>
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

void traverse(struct TreeNode *head);
struct TreeNode* bindSibling(struct TreeNode * left, struct TreeNode * right);
struct TreeNode* bindParent(struct TreeNode *parent, struct TreeNode *child);


typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SyntaxNode_* SyntaxNode;
typedef struct UserType_* UserType;
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
                 FieldList paramTypeList;
                 int paramCount;
        } func;
        Type type;
    } u;
    enum {variable, function } kind;
    SyntaxNode next;
};
struct UserType_
{
    char* name;
    FieldList tail;
    UserType next;
};
SyntaxNode syntax_table;
UserType user_type_table;
void getType(Type type, struct TreeNode* specifier);
void addExtDecList(Type type, struct TreeNode* ExtDecList);
void addFunDec(Type type, struct TreeNode*);
void addDec(Type, struct TreeNode*);
int tokenJudge(char *name);
int findUserType(char *typeName);
int findName(char *name);
int findVariable(char *varName);
int findFunc(char *funcName);
int checkParamType(FieldList, struct TreeNode*);

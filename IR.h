#include "stable.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;
typedef struct ArgList_* ArgList;

struct Operand_ {
    enum {TEMP, VARIABLE, CONSTANT, ADDRESS, LABELOP, OP, REFERENCE} kind;
    union {
        int temp_no;
        int var_no;
        int value;
        int label_no;
        int op_no;
    } u;
};

struct InterCode_ {
    enum { ASSIGN, ADD, SUB, MUL, DIV, RETURN, LABEL_CODE, LABEL_TRUE, LABEL_GOTO, READ, CALLFUNC, WRITE, ARG, FUNCTION, PARAM, REFASSIGN, DEC } kind;
    union {
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;
        struct { Operand op;} returnop;
        struct { Operand t1, op,  t2, label_true; } labeltrue;
        struct { Operand label_goto; } labelgoto;
        struct { Operand label; } labelcode;
        struct { Operand place; } read;
        struct { Operand place; char *name; } callfunc;
        struct { Operand arg; } write;
        struct { Operand arg; } arg;
        struct { char *name; } function;
        struct { Operand param; } param;
        struct { Operand op; int size; } dec;
    } u;
};

struct InterCodes_ {
    InterCode code;
    InterCodes prev, next;
};

struct ArgList_ {
    Operand param;
    ArgList next;
};
/*
 * global variables
 */
InterCodes interCodes;
FILE *fp;

/*
 * global funcitons;
 */
void IROutput();
void IRTraverse(struct TreeNode*);
InterCodes translate_Exp(struct TreeNode*, SyntaxNode, Operand);
InterCodes translate_Stmt(struct TreeNode*, SyntaxNode);
InterCodes translate_CompSt(struct TreeNode*, SyntaxNode);
InterCodes translate_Cond(struct TreeNode*, Operand, Operand, SyntaxNode);
InterCodes translate_Args(struct TreeNode*, SyntaxNode, ArgList);
Operand copyPlace(Operand);
Operand copyLabel(Operand);
void bindCode(InterCodes, InterCodes);
Operand new_label();
InterCodes new_label_code(Operand);
Operand new_temp();
InterCodes new_place_code(Operand, int);
InterCodes arrayAddress(struct TreeNode *Exp, Operand place, Operand pos, Type arrayType);

void DROutput();
int getVarCount();
int getTempCount();

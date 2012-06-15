#include "IR.h"
#include <string.h>
#include <stdlib.h>

FILE *dp;
int TEMPCOUNT;
int VARCOUNT;

typedef struct MemAlloc_* MemAlloc;
struct MemAlloc_ {
    int varCount;
    int space;
    MemAlloc next;
};
MemAlloc memAlloc;
void MemStack()
{
    SyntaxNode itr = syntax_table;
    int num = 1;
    MemAlloc trace;
    while( itr != NULL ){
        printf("%s\n", itr->name);
        if( memAlloc == NULL ){
            if( itr->kind == variable ){
                printf("variable: %s\n", itr->name);
                memAlloc = (MemAlloc)malloc(sizeof(struct MemAlloc_));
                memAlloc->varCount = num;
                num++;
                Type type = itr->u.type;
                printf("type : %d\n", type->kind);
                if( type->kind == basic ){
                    memAlloc->space = 4;
                }
                else if( type->kind == array ){
                    printf("array\n");
                    Type elem = type->u.array.elem;
                    if( elem->kind == basic ){
                        printf("array size: %d\n", type->u.array.size);
                        memAlloc->space = 4 * type->u.array.size;
                    }
                }
                memAlloc->next = NULL;
                trace = memAlloc;
            }
        }
        else {
            if( itr->kind == variable ){
                MemAlloc temp = (MemAlloc)malloc(sizeof(struct MemAlloc_));
                temp->varCount = num;
                num++;
                Type type = itr->u.type;
                if( type->kind == basic ){
                    temp->space = 4;
                }
                else if( type->kind == array ){
                    Type elem = type->u.array.elem;
                    if( elem->kind == basic ){
                        temp->space = 4 * type->u.array.size;
                    }
                }
                temp->next = NULL;
                trace->next = temp;
                trace = temp;
            }
        }
        itr = itr->next;
    }
    MemAlloc it = memAlloc;
    printf("********************* *******************\n");
    while( it != NULL ){
        printf("%d: %d\n", it->varCount, it->space);
        it = it->next;
    }
}
int getMemUse(int varCount)
{
    MemAlloc it = memAlloc;
    int space = 0;
    while( it != NULL ){
        if( it->varCount == varCount )
            break;
        space = space + it->space;
        it = it->next;
    }
    return space;
}
void HeadOutput()
{
    fputs(".data\n",dp);
    fputs("_prompt: .asciiz \"Enter an integer:\"\n",dp);
    fputs("_ret: .asciiz \"\\n\"\n",dp);
    fputs(".globl main\n",dp);
    fputs(".text\n",dp);
}
void ReadFuncOutput()
{
    fputs("read:\n\tli $v0, 4\t\n", dp);
    fputs("la $a0, _prompt\n\tsyscall\n",dp);
    fputs("\tli $v0, 5\n\tsyscall\n\tjr $ra\n\n",dp);
}
void WriteFuncOutput()
{
    fputs("write:\n\tli $v0, 1\n", dp);
    fputs("\tsyscall\n\tli $v0, 4\n", dp);
    fputs("\tla $a0, _ret\n",dp);
    fputs("\tsyscall\n\tmove $v0, $0\n", dp);
    fputs("\tjr $ra\n\n",dp);
}

/*
 * create the DC
 */
void ReadDC(InterCodes itc)
{
    Operand place = itc->code->u.read.place;
    fputs("\taddi $sp, $sp, -4\n", dp);
    fputs("\tsw, $ra 0($sp)\n", dp);
    fputs("\tjal read\n", dp);
    fputs("\tlw $ra, 0($sp)\n", dp);
    fputs("\taddi $sp, $sp, 4\n", dp);
    fputs("\tmove $t1, $v0\n",dp);
    fprintf(dp, "\tsw $t1, %d($sp)\n", (place->u.temp_no-1)*4);
}
void WriteDC(InterCodes itc)
{
    Operand arg = itc->code->u.write.arg;
    fprintf(dp, "\tlw $t1, %d($sp)\n", (arg->u.temp_no-1)*4);
    fputs("\tmove $a0, $t1\n", dp);
    fputs("\taddi $sp, $sp, -4\n", dp);
    fputs("\tsw, $ra 0($sp)\n", dp);
    fputs("\tjal write\n", dp);
    fputs("\tlw $ra, 0($sp)\n", dp);
    fputs("\taddi $sp, $sp, 4\n", dp);
}
void ReturnDC(InterCodes itc)
{
    Operand op = itc->code->u.returnop.op;
    fprintf(dp, "\tlw $t1, %d($sp)\n", (op->u.temp_no-1)*4);
    fputs("\tmove $v0, $t1\n",dp);
    fprintf(dp, "\taddi $sp, $sp, %d\n", (TEMPCOUNT+VARCOUNT)*4);
    fputs("\tjr $ra\n", dp);
}
void FunctionDC(InterCodes itc)
{
    fprintf(dp, "\n%s:\n", itc->code->u.function.name);
    fprintf(dp, "\taddi $sp, $sp, -%d\n", (TEMPCOUNT+VARCOUNT)*4);
    InterCodes param = itc->next;
    int paramCount = 0;
    while( param->code->kind == PARAM ){
        paramCount++;
        param = param->next;
    }
    if( paramCount != 0 ) {
        param = itc->next;
        int i;
        for( i = 0; i <  paramCount; i++ ){
            Operand place = param->code->u.param.param;
            fprintf(dp, "\tsw $a%d, %d($sp)\n", i, (place->u.var_no-1+TEMPCOUNT)*4);
            param = param->next;
        }
    }
}
void AssignDC(InterCodes itc)
{
    Operand right = itc->code->u.assign.right;
    Operand left = itc->code->u.assign.left;
    if( left->kind == TEMP )
        fprintf(dp, "\tlw $t1, %d($sp)\n", (left->u.temp_no-1)*4);
    else if( left->kind == VARIABLE )
        fprintf(dp, "\tlw $t1, %d($sp)\n", (left->u.var_no-1+TEMPCOUNT)*4);
        
    if( right->kind == CONSTANT )
        fprintf(dp, "\tli $t1, %d\n", right->u.value);
    else if( right->kind == VARIABLE ){ 
        fprintf(dp, "\tlw $t2, %d($sp)\n", (right->u.var_no-1+TEMPCOUNT)*4);
        fputs("\tmove $t1, $t2\n",dp);
    }
    else if( right->kind == TEMP ){
        fprintf(dp, "\tlw $t2, %d($sp)\n", (right->u.temp_no-1)*4);
        fputs("\tmove $t1, $t2\n",dp);
    }
    /*
     * write back to memory
     */
    if( left->kind == TEMP )
        fprintf(dp, "\tsw $t1, %d($sp)\n", (left->u.temp_no-1)*4);
    else if( left->kind == VARIABLE )
        fprintf(dp, "\tsw $t1, %d($sp)\n", (left->u.var_no-1+TEMPCOUNT)*4);

    if( right->kind == VARIABLE ) 
        fprintf(dp, "\tsw $t2, %d($sp)\n", (right->u.var_no-1+TEMPCOUNT)*4);
    else if( right->kind == TEMP )
        fprintf(dp, "\tsw $t2, %d($sp)\n", (right->u.temp_no-1)*4);
}
void AddDC(InterCodes itc)
{
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;

    if( op1->kind == TEMP )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.temp_no-1)*4);
    else if( op1->kind == VARIABLE )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.var_no-1+TEMPCOUNT)*4);

    if( op2->kind == CONSTANT )
        fprintf(dp, "\taddi $t1, $t2, %d\n", op2->u.temp_no);
    else{
        if( op2->kind == TEMP )
            fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.temp_no-1)*4);
        else if( op2->kind == VARIABLE )
            fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.var_no-1+TEMPCOUNT)*4);

        fputs("\tadd $t1, $t2, $t3\n", dp);
    }

    /*
     * write back to memory
     */

    if( result->kind == VARIABLE ) 
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.var_no-1+TEMPCOUNT)*4);
    else if( result->kind == TEMP )
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.temp_no-1)*4);
}
void SubDC(InterCodes itc)
{
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;

    if( op1->kind == TEMP )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.temp_no-1)*4);
    else if( op1->kind == VARIABLE )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.var_no-1+TEMPCOUNT)*4);

    if( op2->kind == CONSTANT )
        fprintf(dp, "\taddi $t1, $t2, -%d\n", op2->u.temp_no);
    else{
        if( op2->kind == TEMP )
            fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.temp_no-1)*4);
        else if( op2->kind == VARIABLE )
            fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.var_no-1+TEMPCOUNT)*4);

        fputs("\tsub $t1, $t2, $t3\n", dp);
    }

    /*
     * write back to memory
     */

    if( result->kind == VARIABLE ) 
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.var_no-1+TEMPCOUNT)*4);
    else if( result->kind == TEMP )
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.temp_no-1)*4);
}
void MulDC(InterCodes itc)
{
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;

    if( op1->kind == TEMP )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.temp_no-1)*4);
    else if( op1->kind == VARIABLE )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.var_no-1+TEMPCOUNT)*4);

    if( op2->kind == TEMP )
        fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.temp_no-1)*4);
    else if( op2->kind == VARIABLE )
        fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.var_no-1+TEMPCOUNT)*4);

    fputs("\tmul $t1, $t2, $t3\n", dp);
    

    /*
     * write back to memory
     */

    if( result->kind == VARIABLE ) 
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.var_no-1+TEMPCOUNT)*4);
    else if( result->kind == TEMP )
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.temp_no-1)*4);
}
void DivDC(InterCodes itc)
{
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;

    if( op1->kind == TEMP )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.temp_no-1)*4);
    else if( op1->kind == VARIABLE )
        fprintf(dp, "\tlw $t2, %d($sp)\n", (op1->u.var_no-1+TEMPCOUNT)*4);

    if( op2->kind == TEMP )
        fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.temp_no-1)*4);
    else if( op2->kind == VARIABLE )
        fprintf(dp, "\tlw $t3, %d($sp)\n", (op2->u.var_no-1+TEMPCOUNT)*4);

    fputs("\tdiv $t2, $t3\n", dp);
    fputs("\tmflo $t1\n", dp);

    /*
     * write back to memory
     */

    if( result->kind == VARIABLE ) 
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.var_no-1+TEMPCOUNT)*4);
    else if( result->kind == TEMP )
        fprintf(dp, "\tsw $t1, %d($sp)\n", (result->u.temp_no-1)*4);
}
void LabelGotoDC(InterCodes itc)
{
    Operand label = itc->code->u.labelgoto.label_goto;
    fprintf(dp, "\tj label%d\n", label->u.label_no);
}
void LabelTrueDC(InterCodes itc)
{
     Operand t1 = itc->code->u.labeltrue.t1;
     Operand op = itc->code->u.labeltrue.op;
     Operand t2 = itc->code->u.labeltrue.t2;
     Operand label_true = itc->code->u.labeltrue.label_true;

     if( t1->kind == TEMP )
         fprintf(dp, "\tlw $t1, %d($sp)\n", (t1->u.temp_no-1)*4);
     else if( t1->kind == VARIABLE )
         fprintf(dp, "\tlw $t1, %d($sp)\n", (t1->u.var_no-1+TEMPCOUNT)*4);

     if( t2->kind == TEMP )
         fprintf(dp, "\tlw $t2, %d($sp)\n", (t2->u.temp_no-1)*4);
     else if( t2->kind == VARIABLE )
         fprintf(dp, "\tlw $t2, %d($sp)\n", (t2->u.var_no-1+TEMPCOUNT)*4);

     if( op->u.op_no == 1 )
         fputs("\tbgt ", dp);
     else if( op->u.op_no == 2)
         fputs("\tblt ", dp);
     else if( op->u.op_no == 3)
         fputs("\tbge ", dp);
     else if( op->u.op_no == 4)
         fputs("\tble ", dp);
     else if( op->u.op_no == 5)
         fputs("\tbeq ", dp);
     else if( op->u.op_no == 6)
         fputs("\tbne ", dp);

    
    fprintf(dp, "$t1, $t2, label%d\n", label_true->u.label_no);
}
void LabelCodeDC(InterCodes itc)
{
    Operand label = itc->code->u.labelcode.label;
    fprintf(dp, "label%d:\n", label->u.label_no);
}
void CallFuncDC(InterCodes itc)
{
    Operand place = itc->code->u.callfunc.place;
    /*
     * Arg
     */
    int argCount = 0;
    InterCodes arg = itc->prev;
    while( arg->code->kind == ARG ) {
        argCount++;
        arg = arg->prev;
    }
    if( argCount == 0 )
    {}
    else{
        /*
         * TODO we can only use $a0 ~ $a3
         */
        int i;
        for( i = 0; i < argCount; i++ ){
            arg = arg->next;
            Operand argop = arg->code->u.arg.arg;
            fprintf(dp, "\tlw $t1, %d($sp)\n", (argop->u.temp_no-1)*4);
            fprintf(dp, "\tmove $a%d, $t1\n", i);
        }
    }
    /*
     * save context
     */
    fputs("\taddi $sp, $sp, -4\n", dp);
    fputs("\tsw, $ra 0($sp)\n", dp);
    /*
     * call func 
     */
    fprintf(dp, "\tjal %s\n", itc->code->u.callfunc.name);
    fputs("\tmove $t1, $v0\n", dp);
    /*
     * restore context
     */
    fputs("\tlw $ra, 0($sp)\n", dp);
    fputs("\taddi $sp, $sp, 4\n", dp);
    /*
     * save the return value to the lvalue
     */
    fprintf(dp, "\tsw $t1, %d($sp)\n", (place->u.temp_no-1)*4);

}
void DROutput()
{
    dp = fopen("output.s", "w+");
    TEMPCOUNT = getTempCount();
    VARCOUNT = getVarCount();
    memAlloc = NULL;
    MemStack();
    HeadOutput();
    ReadFuncOutput();
    WriteFuncOutput();
    InterCodes itr = interCodes;
    while( itr != NULL ){
        if( itr->code->kind == READ )
            ReadDC(itr);
        else if( itr->code->kind == ASSIGN )
            AssignDC(itr);
        else if( itr->code->kind == FUNCTION)
            FunctionDC(itr);
        else if( itr->code->kind == ADD )
            AddDC(itr);
        else if( itr->code->kind == WRITE )
            WriteDC(itr);
        else if( itr->code->kind == RETURN )
            ReturnDC(itr);
        else if ( itr->code->kind == SUB )
            SubDC(itr);
        else if ( itr->code->kind == MUL )
            MulDC(itr);
        else if ( itr->code->kind == DIV )
            DivDC(itr);
        else if ( itr->code->kind == LABEL_GOTO )
            LabelGotoDC(itr);
        else if ( itr->code->kind == LABEL_TRUE ) 
            LabelTrueDC(itr);
        else if ( itr->code->kind == LABEL_CODE )
            LabelCodeDC(itr);
        else if ( itr->code->kind == CALLFUNC )
            CallFuncDC(itr);

        itr = itr->next;
        if( itr == interCodes )
            itr = NULL;
    }
    fclose(dp);
}

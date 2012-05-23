#include "IR.h"
#include <string.h>
#include <stdlib.h>
/*
 * local variables
 */
int tempVarCount = 0;
int labelCount = 0;
void init()
{
    printf("------- initial entry ------\n");
    Operand op1 = (Operand)malloc(sizeof(struct Operand_));
    Operand op2 = (Operand)malloc(sizeof(struct Operand_));
    op1->kind = VARIABLE;
    op1->u.var_no = 1;
    op2->kind = CONSTANT;
    op2->u.value = 2;
    InterCode itc = (InterCode)malloc(sizeof(struct InterCode_));
    itc->kind = ASSIGN;
    itc->u.assign.right = op2;
    itc->u.assign.left = op1;
    interCodes = (InterCodes)malloc(sizeof(struct InterCodes_));
    interCodes->code = itc;
    interCodes->next = interCodes;
    interCodes->prev = interCodes;
}

int lookup(struct TreeNode* ID)
{
    SyntaxNode itr = syntax_table;
    int num = 0; 
    while( itr != NULL ){
        printf("#\n");
        if( itr->kind == variable){
            printf("num: %d\n", num);
            num++;
        }
        if( strcmp(itr->name, ID->value) == 0){
            break;
        }
        itr = itr->next;
    }
    return num;
}
SyntaxNode lookupfunc(struct TreeNode* ID)
{
    SyntaxNode itr = syntax_table;
    while( itr != NULL ){
        if( itr->kind == function){
            if( strcmp(itr->name, ID->value) == 0)
                return itr;
        }
        itr = itr->next;
    }
}
Operand get_relop(struct TreeNode* RELOP)
{
    Operand relop = (Operand)malloc(sizeof(struct Operand_));
    relop->kind = OP;
    if( strcmp(RELOP->value, ">" ) == 0 )
        relop->u.op_no = 1;
    if( strcmp(RELOP->value, "<" ) == 0 )
        relop->u.op_no = 2;
    if( strcmp(RELOP->value, ">=" ) == 0 )
        relop->u.op_no = 3;
    if( strcmp(RELOP->value, "<=" ) == 0 )
        relop->u.op_no = 4;
    if( strcmp(RELOP->value, "==" ) == 0 )
        relop->u.op_no = 5;
    if( strcmp(RELOP->value, "!=" ) == 0 )
        relop->u.op_no = 6;
    return relop;
}
Operand new_temp()
{
        printf("new_temp \n");
        Operand t1 = (Operand)malloc(sizeof(struct Operand_));
        t1->kind = TEMP;
        t1->u.temp_no = ++tempVarCount;
        return t1;
}
InterCodes new_place_code(Operand place, int value)
{
    /*
     * place := #value
     */
    InterCodes result = (InterCodes)malloc(sizeof(struct InterCodes_));
    result->prev = result->next = result;
    result->code = (InterCode)malloc(sizeof(struct InterCode_));
    result->code->kind = ASSIGN;
    result->code->u.assign.left = copyPlace(place);
    result->code->u.assign.right = (Operand)malloc(sizeof(struct Operand_));
    result->code->u.assign.right->kind = CONSTANT;
    result->code->u.assign.right->u.value = value;
    return result;
}

Operand new_label()
{
        Operand t1 = (Operand)malloc(sizeof(struct Operand_));
        t1->kind = LABELOP;
        t1->u.label_no = ++labelCount;
        return t1;
}
InterCodes new_label_code(Operand label1)
{
    InterCodes result = (InterCodes)malloc(sizeof(struct InterCodes_));
    result->prev = result->next = result;
    result->code = (InterCode)malloc(sizeof(struct InterCode_));
    result->code->kind = LABEL_CODE;
    result->code->u.labelcode.label = copyLabel(label1);
    return result;
}
Operand get_arg(ArgList arg_list, int num)
{
    ArgList itr = arg_list;
    int i;
    for( i = 1; i < num; ++i ){
        itr = itr->next;
    }
    return itr->param;
}
void bindCode(InterCodes code1, InterCodes code2)
{
    InterCodes code1_tail = code1->prev;
    code1_tail->next = code2;
    code1->prev = code2->prev;
    code2->prev->next = code1;
    code2->prev = code1_tail;
}
Operand copyPlace(Operand t1)
{
    Operand result = (Operand)malloc(sizeof(struct Operand_));
    result->kind = t1->kind;
    result->u.temp_no = t1->u.temp_no;
    return result;
}
Operand copyLabel(Operand label)
{
    Operand result = (Operand)malloc(sizeof(struct Operand_));
    result->kind = LABELOP;
    result->u.label_no = label->u.label_no;
    return result;
}
int length(ArgList arg_list)
{
    ArgList itr = arg_list;
    int count = 0;
    while( itr != NULL ){
        ++count;
        itr = itr->next;
    }
    return count;
}
/*
 * Intermediate representation implementation
 */
InterCodes translate_Args(struct TreeNode*Args, SyntaxNode sym_table, ArgList arg_list)
{
    printf("tranlsate_Args entry\n");
    Operand t1 = new_temp();
    InterCodes code1 = translate_Exp(Args->firstChild, sym_table, t1);
    if( arg_list->param == NULL ){
        arg_list->param = copyPlace(t1);
        arg_list->next = NULL;
    }
    else {
        ArgList itr = arg_list;
        while( itr->next != NULL )
            itr = itr->next;
        itr->next = (ArgList)malloc(sizeof(struct ArgList_));
        itr->next->param = copyPlace(t1);
        itr->next->next = NULL;
    }
    if( Args->firstChild->nextSibling == NULL ) {
       return code1;
    }
    else{
        InterCodes code2 = translate_Args(Args->firstChild->nextSibling->nextSibling, sym_table, arg_list);
        bindCode(code1, code2);
        return code1;
    }
}
InterCodes translate_Exp(struct TreeNode* Exp, SyntaxNode sym_table , Operand place )
{
    printf("Exp entry\n");
    
    if( strcmp( Exp->firstChild->token, "ID" ) == 0 ){
        printf("ID\n");
        struct TreeNode * ID = Exp->firstChild;
        if(ID->nextSibling == NULL ){
            if( place == NULL ){
                printf("new _ temp () \n");
                place = new_temp();
            }
            InterCodes itcs = new_place_code(place, lookup(Exp->firstChild));
            itcs->code->u.assign.right->kind = VARIABLE;
            itcs->code->u.assign.right->u.var_no = lookup(Exp->firstChild);
            return itcs;
        }
        else{
            if(strcmp( ID->nextSibling->nextSibling->token, "RP" ) == 0 ){
                if( place == NULL ){
                    printf("new _ temp () \n");
                    place = new_temp();
                }
                SyntaxNode function = lookupfunc(ID);
                if( strcmp(function->name, "read") == 0 ) {
                    InterCodes readCode = (InterCodes)malloc(sizeof(struct InterCodes_));
                    readCode->prev = readCode->next = readCode;
                    readCode->code = (InterCode)malloc(sizeof(struct InterCode_));
                    readCode->code->kind = READ;
                    readCode->code->u.read.place = copyPlace(place);
                    return readCode;
                }
                else {
                    InterCodes readCode = (InterCodes)malloc(sizeof(struct InterCodes_));
                    readCode->prev = readCode->next = readCode;
                    readCode->code = (InterCode)malloc(sizeof(struct InterCode_));
                    readCode->code->kind = CALLFUNC;
                    readCode->code->u.callfunc.place = copyPlace(place);
                    readCode->code->u.callfunc.name = malloc(32);
                    memcpy(readCode->code->u.callfunc.name, function->name+'\0', 32);
                    return readCode;
                }
            }
            else{ //ID LP Args RP
                printf("Args\n");
                SyntaxNode function = lookupfunc(ID);
                ArgList arg_list = (ArgList)malloc(sizeof(struct ArgList_));
                arg_list->param = NULL;
                InterCodes code1 = translate_Args(ID->nextSibling->nextSibling, sym_table, arg_list);
                printf("%d\n", code1);
                if( strcmp(function->name, "write") == 0 ) {
                    printf("write\n");
                    InterCodes writeCode = (InterCodes)malloc(sizeof(struct InterCodes_));
                    writeCode->prev = writeCode->next = writeCode;
                    writeCode->code = (InterCode)malloc(sizeof(struct InterCode_));
                    writeCode->code->kind = WRITE;
                    writeCode->code->u.write.arg = copyPlace(get_arg(arg_list, 1));
                    bindCode(code1, writeCode);
                    return code1;
                }
                else {
                    if( place == NULL ){
                        printf("new _ temp () \n");
                        place = new_temp();
                    }
                    printf("function not write\n");
                    int len = length(arg_list);
                    int i;
                    InterCodes code2 = NULL;
                    printf("len : %d \n", len);
                    for( i = 1; i <= len; ++i ){
                        InterCodes argCode = (InterCodes)malloc(sizeof(struct InterCodes_));
                        argCode->prev =argCode->next = argCode;
                        argCode->code = (InterCode)malloc(sizeof(struct InterCode_));
                        argCode->code->kind = ARG;
                        argCode->code->u.arg.arg = copyPlace(get_arg(arg_list, i));
                        if ( code2 == NULL ){
                            printf("code2 null\n");
                            code2 = argCode;
                        }
                        else{
                            bindCode(code2, argCode);
                        }
                    }
                    printf("Add finish\n");
                    bindCode(code1, code2);

                    printf("bind finish\n");
                    InterCodes callfunc = (InterCodes)malloc(sizeof(struct InterCodes_));
                    callfunc->prev = callfunc->next = callfunc;
                    callfunc->code = (InterCode)malloc(sizeof(struct InterCode_));
                    callfunc->code->kind = CALLFUNC;
                    callfunc->code->u.callfunc.place = copyPlace(place);
                    callfunc->code->u.callfunc.name = malloc(32);
                    printf("function name : %s\n", function->name);
                    memcpy(callfunc->code->u.callfunc.name, function->name+'\0', 32);
                    printf("memcpy\n");
                    bindCode(code1, callfunc);
                    return code1;
                }
            }
        }
    }
    if( place == NULL ){
        printf("new _ temp () \n");
        place = new_temp();
    }
    if( strcmp( Exp->firstChild->token, "INT" ) == 0 ){
        printf("INT\n");
        InterCodes itcs = (InterCodes)malloc(sizeof(struct InterCodes_));
        itcs->code = (InterCode)malloc(sizeof(struct InterCode_));
        itcs->prev = itcs;
        itcs->next = itcs;
        itcs->code->kind = ASSIGN;
        itcs->code->u.assign.right = (Operand)malloc(sizeof(struct Operand_));
        itcs->code->u.assign.left = place;
        printf("place: %d\n", place->u.temp_no);
        itcs->code->u.assign.right->kind = CONSTANT;
        itcs->code->u.assign.right->u.value = Exp->firstChild->ival;
        return itcs;
    }
    if( strcmp( Exp->firstChild->token, "MINUS" ) == 0 ){
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(Exp->firstChild->nextSibling, syntax_table, t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->code = (InterCode)malloc(sizeof(struct InterCode_));
        code2->code->kind = SUB;
        code2->code->u.binop.result = place;
        code2->code->u.binop.op1 = (Operand)malloc(sizeof(struct Operand_));
        code2->code->u.binop.op2 = (Operand)malloc(sizeof(struct Operand_));
        code2->code->u.binop.op1->kind = CONSTANT;
        code2->code->u.binop.op1->u.value = 0;
        code2->code->u.binop.op2 = copyPlace(t1);
        code2->prev = code2->next = code2;

        bindCode(code1, code2);
        return code1;
    }
    if( strcmp( Exp->firstChild->token, "NOT" ) == 0 ){
        Operand label1 = new_label();
        Operand label2 = new_label();
        InterCodes code0 = new_place_code(place, 0);
        InterCodes code1 = translate_Cond(Exp, label1, label2, sym_table);
        InterCodes code2 = new_label_code(label1);
        bindCode(code2, new_place_code(place, 1));
        bindCode(code0, code1);
        bindCode(code0, code2);
        bindCode(code0, new_label_code(label2));
        return code0;
    }
    if( strcmp( Exp->firstChild->token, "LP") == 0 ){
        printf("Exp LP\n");
        return translate_Exp(Exp->firstChild->nextSibling, sym_table, place);
    }
    /*
     * the firstChild of the current node is Exp
     */
    if( strcmp( Exp->firstChild->token, "Exp") == 0 ){
        struct TreeNode* Exp1, *Exp2;
        Exp1 = Exp->firstChild;
        Exp2 = Exp1->nextSibling->nextSibling;
        if( strcmp(Exp1->nextSibling->token, "ASSIGNOP") == 0 ){
            printf("translate: exp assignop exp\n");

            Operand t1 = new_temp();
            printf("temp: t%d\n", t1->u.temp_no);
            InterCodes code1 = translate_Exp(Exp2, syntax_table, t1);

            InterCodes code2_1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            InterCodes code2_2 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code2_1->code = (InterCode)malloc(sizeof(struct InterCode_));
            code2_1->code->kind = ASSIGN;
            code2_1->code->u.assign.right = (Operand)malloc(sizeof(struct Operand_));
            code2_1->code->u.assign.right = copyPlace(t1);
            code2_1->code->u.assign.left = (Operand)malloc(sizeof(struct Operand_));
            code2_1->code->u.assign.left->kind = VARIABLE;
            /*
             * TODO Only Exp -> ID is considered
             */
            code2_1->code->u.assign.left->u.var_no = lookup(Exp1);
            code2_1->prev = code2_1->next = code2_1;

            code2_2->code = (InterCode)malloc(sizeof(struct InterCode_));
            code2_2->code->kind = ASSIGN;
            code2_2->code->u.assign.left = place;
            code2_2->code->u.assign.right = (Operand)malloc(sizeof(struct Operand_));
            code2_2->code->u.assign.right->kind = VARIABLE;
            code2_2->code->u.assign.right->u.var_no = lookup(Exp1);
            code2_2->prev = code2_2->next = code2_2;

            bindCode(code2_1, code2_2);
            /*
             * code1 + code2
             */
            bindCode(code1, code2_1);
            return code1;
        }
        if( strcmp(Exp1->nextSibling->token, "PLUS") == 0 ){
            printf("PLUS\n");
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            InterCodes code1 = translate_Exp(Exp1, syntax_table, t1);
            InterCodes code2 = translate_Exp(Exp2, syntax_table, t2);
            InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->code = (InterCode)malloc(sizeof(struct InterCode_));
            code3->code->kind = ADD;
            code3->code->u.binop.result = place;
            code3->code->u.binop.op1 = copyPlace(t1);
            code3->code->u.binop.op2 = copyPlace(t2);
            code3->prev = code3->next = code3;
            bindCode(code1, code2);
            bindCode(code1, code3);
            return code1;
        }
        if( strcmp(Exp1->nextSibling->token, "MINUS") == 0 ){
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            InterCodes code1 = translate_Exp(Exp1, syntax_table, t1);
            InterCodes code2 = translate_Exp(Exp2, syntax_table, t2);
            InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->code = (InterCode)malloc(sizeof(struct InterCode_));
            code3->code->kind = SUB;
            code3->code->u.binop.result = place;
            code3->code->u.binop.op1 = copyPlace(t1);
            code3->code->u.binop.op2 = copyPlace(t2);
            code3->prev = code3->next = code3;
            bindCode(code1, code2);
            bindCode(code1, code3);
            return code1;
        }
        if( strcmp(Exp1->nextSibling->token, "STAR") == 0 ){ 
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            InterCodes code1 = translate_Exp(Exp1, syntax_table, t1);
            InterCodes code2 = translate_Exp(Exp2, syntax_table, t2);
            InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->code = (InterCode)malloc(sizeof(struct InterCode_));
            code3->code->kind = MUL;
            code3->code->u.binop.result = place;
            code3->code->u.binop.op1 = copyPlace(t1);
            code3->code->u.binop.op2 = copyPlace(t2);
            code3->prev = code3->next = code3;
            bindCode(code1, code2);
            bindCode(code1, code3);
            return code1;

        }
        if( strcmp(Exp1->nextSibling->token, "DIV") == 0 ){ 
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            InterCodes code1 = translate_Exp(Exp1, syntax_table, t1);
            InterCodes code2 = translate_Exp(Exp2, syntax_table, t2);
            InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->code = (InterCode)malloc(sizeof(struct InterCode_));
            code3->code->kind = DIV;
            code3->code->u.binop.result = place;
            code3->code->u.binop.op1 = copyPlace(t1);
            code3->code->u.binop.op2 = copyPlace(t2);
            code3->prev = code3->next = code3;
            bindCode(code1, code2);
            bindCode(code1, code3);
            return code1;

        }
        if( strcmp(Exp1->nextSibling->token, "RELOP") == 0 || strcmp(Exp1->nextSibling->token, "AND") == 0 || strcmp(Exp1->nextSibling->token, "AND") == 0){
            Operand label1 = new_label();
            Operand label2 = new_label();
            InterCodes code0 = new_place_code(place, 0);
            InterCodes code1 = translate_Cond(Exp, label1, label2, sym_table);
            InterCodes code2 = new_label_code(label1);
            bindCode(code2, new_place_code(place, 1));
            bindCode(code0, code1);
            bindCode(code0, code2);
            bindCode(code0, new_label_code(label2));
            return code0;   
        }
    }
}
InterCodes translate_CompSt(struct TreeNode* CompSt, SyntaxNode sym_table)
{
    struct TreeNode* StmtList = CompSt->firstChild->nextSibling->nextSibling;
    struct TreeNode* Stmt = StmtList->firstChild;
    InterCodes code1 = translate_Stmt(Stmt, sym_table);
    StmtList = Stmt->nextSibling;
    
    InterCodes code2;
    while(StmtList->token != NULL ){
        Stmt = StmtList->firstChild;
        code2 = translate_Stmt(Stmt, sym_table);
        bind(code1, code2);
        StmtList = Stmt->nextSibling;
    }
    
    return code1;
}
InterCodes translate_Cond(struct TreeNode* Exp, Operand label_true, Operand label_false, SyntaxNode sym_table) {
    printf("translate_Cond\n");
    struct TreeNode* first = Exp->firstChild;
    if( strcmp( first->token, "NOT" ) == 0 ){
        printf("NOT\n");
        return translate_Cond(first->nextSibling, label_false, label_true, sym_table);
    }
    if ( strcmp( first->token, "Exp" ) == 0 ){
        struct TreeNode * second = first->nextSibling;
        if( strcmp( second->token, "RELOP" ) == 0 ) {
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            printf("Cond : \n");
            printf("t: %d \n", t1->u.temp_no);
            InterCodes code1 = translate_Exp(first, sym_table, t1);
            printf("var no %d\n", code1->code->u.assign.right->u.var_no);
            InterCodes code2 = translate_Exp(first->nextSibling->nextSibling, sym_table, t2);
            Operand op = get_relop(second);
            /*
             *  code3 construct
             */

            InterCodes code3 = (InterCodes)malloc(sizeof( struct InterCodes_));
            code3->prev = code3->next = code3;
            code3->code = (InterCode)malloc(sizeof( struct InterCode_));
            code3->code->kind = LABEL_TRUE;
            code3->code->u.labeltrue.t1 = copyPlace(t1);
            code3->code->u.labeltrue.t2 = copyPlace(t2);
            code3->code->u.labeltrue.op = op;
            code3->code->u.labeltrue.label_true = copyLabel(label_true);

            bindCode(code1, code2);
            bindCode(code1, code3);
            /*
             * lable false code_false
             */
            InterCodes goto_false = (InterCodes)malloc(sizeof( struct InterCodes_ ));
            goto_false->prev = goto_false->next = goto_false;
            goto_false->code = (InterCode)malloc(sizeof( struct InterCode_));
            goto_false->code->kind = LABEL_GOTO;
            goto_false->code->u.labelgoto.label_goto = copyLabel(label_false);

            bindCode(code1, goto_false);
            return code1;
        }
        if( strcmp( second->token, "AND" ) == 0 ) {
            Operand label1 = new_label();
            InterCodes code1 = translate_Cond(first, label1, label_false, sym_table);
            InterCodes code2 = translate_Cond(second->nextSibling, label_true, label_false, sym_table);
            InterCodes labelCode = (InterCodes)malloc(sizeof(struct InterCodes_));
            labelCode->prev = labelCode->next = labelCode;
            labelCode->code = (InterCode)malloc(sizeof(struct InterCode_));
            labelCode->code->kind = LABEL_CODE;
            labelCode->code->u.labelcode.label = copyLabel(label1);

            bindCode(code1, labelCode);
            bindCode(code1, code2);
            return code1;
        }
        if( strcmp( second->token, "OR" ) == 0 ){
            Operand label1 = new_label();
            InterCodes code1 = translate_Cond(first, label_true, label1, sym_table);
            InterCodes code2 = translate_Cond(second->nextSibling, label_true, label_false, sym_table);

            InterCodes labelCode = (InterCodes)malloc(sizeof(struct InterCodes_));
            labelCode->prev = labelCode->next = labelCode;
            labelCode->code = (InterCode)malloc(sizeof(struct InterCode_));
            labelCode->code->kind = LABEL_CODE;
            labelCode->code->u.labelcode.label = copyLabel(label1);

            bindCode(code1, labelCode);
            bindCode(code1, code2);
            return code1;
        }
    }
    /*
     * other cases
     */
    Operand t1 = new_temp();
    InterCodes code1 = translate_Exp(Exp, sym_table, t1);
    
    InterCodes code2 = (InterCodes)malloc(sizeof( struct InterCodes_));
    code2->prev = code2->next = code2;
    code2->code = (InterCode)malloc(sizeof( struct InterCode_));
    code2->code->kind = LABEL_TRUE;
    code2->code->u.labeltrue.t1 = copyPlace(t1);
    // #0
    code2->code->u.labeltrue.t2 = (Operand)malloc(sizeof(struct Operand_));
    code2->code->u.labeltrue.t2->kind = CONSTANT;
    code2->code->u.labeltrue.t2->u.value = 0;

    //  op == "!="
    code2->code->u.labeltrue.op = (Operand)malloc(sizeof(struct Operand_));
    code2->code->u.labeltrue.op->kind = OP;
    code2->code->u.labeltrue.op->u.op_no = 6;

    code2->code->u.labeltrue.label_true = copyLabel(label_true);

    InterCodes goto_false = (InterCodes)malloc(sizeof( struct InterCodes_ ));
    goto_false->prev = goto_false->next = goto_false;
    goto_false->code = (InterCode)malloc(sizeof( struct InterCode_));
    goto_false->code->kind = LABEL_GOTO;
    goto_false->code->u.labelgoto.label_goto = copyLabel(label_false);

    bindCode(code1, code2);
    bindCode(code1, goto_false);
    return code1;
}
InterCodes translate_Stmt(struct TreeNode* Stmt, SyntaxNode sym_table){
    printf("translate Stmt\n");
    printf("%s\n", Stmt->firstChild->token);
    if( strcmp( Stmt->firstChild->token, "Exp") == 0 ){
        return translate_Exp(Stmt->firstChild, sym_table, NULL);
    }
    if( strcmp( Stmt->firstChild->token, "CompSt") == 0 ){
        return translate_CompSt(Stmt->firstChild, sym_table);
    }
    if( strcmp( Stmt->firstChild->token, "RETURN") == 0 ){
        printf("-------------Return-------------\n");
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(Stmt->firstChild->nextSibling, sym_table, t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->code = (InterCode)malloc(sizeof(struct InterCode_));
        code2->code->kind = RETURN;
        code2->code->u.returnop.op = copyPlace(t1);
        code2->prev = code2->next = code2;
        bindCode(code1, code2);
        return code1;
    }
    if( strcmp( Stmt->firstChild->token, "IF") == 0 ){
        struct TreeNode* Exp = Stmt->firstChild->nextSibling->nextSibling;
        struct TreeNode* Stmt1 = Exp->nextSibling->nextSibling;
        if( Stmt1->nextSibling == NULL ){
            printf("Stmt1->nextSibling\n");
            Operand label1 = new_label();
            Operand label2 = new_label();
            InterCodes code1 = translate_Cond(Exp, label1, label2, sym_table);
            InterCodes code2 = translate_Stmt(Stmt1, sym_table);

            InterCodes codelabel1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            codelabel1->prev = codelabel1->next = codelabel1;
            codelabel1->code = (InterCode)malloc(sizeof(struct InterCode_));
            codelabel1->code->kind = LABEL_CODE;
            codelabel1->code->u.labelcode.label = copyLabel(label1);

            InterCodes codelabel2 = (InterCodes)malloc(sizeof(struct InterCodes_));
            codelabel2->prev = codelabel2->next = codelabel2;
            codelabel2->code = (InterCode)malloc(sizeof(struct InterCode_));
            codelabel2->code->kind = LABEL_CODE;
            codelabel2->code->u.labelcode.label = copyLabel(label2);
    
            bindCode(code1, codelabel1);
            bindCode(code1, code2);
            bindCode(code1, codelabel2);
            return code1;
        }
        else{
            struct TreeNode* Stmt2 = Stmt1->nextSibling->nextSibling;
            Operand label1 = new_label();
            Operand label2 = new_label();
            Operand label3 = new_label();
            InterCodes code1 = translate_Cond(Exp, label1, label2, sym_table);
            InterCodes code2 = translate_Stmt(Stmt1, sym_table);
            InterCodes code3 = translate_Stmt(Stmt2, sym_table);

            InterCodes codelabel1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            codelabel1->prev = codelabel1->next = codelabel1;
            codelabel1->code = (InterCode)malloc(sizeof(struct InterCode_));
            codelabel1->code->kind = LABEL_CODE;
            codelabel1->code->u.labelcode.label = copyLabel(label1);

            InterCodes codelabel2 = (InterCodes)malloc(sizeof(struct InterCodes_));
            codelabel2->prev = codelabel2->next = codelabel2;
            codelabel2->code = (InterCode)malloc(sizeof(struct InterCode_));
            codelabel2->code->kind = LABEL_CODE;
            codelabel2->code->u.labelcode.label = copyLabel(label2);

            InterCodes gotolabel = (InterCodes)malloc(sizeof(struct InterCodes_));
            gotolabel->next = gotolabel->prev = gotolabel;
            gotolabel->code = (InterCode)malloc(sizeof(struct InterCode_));
            gotolabel->code->kind = LABEL_GOTO;
            gotolabel->code->u.labelgoto.label_goto = copyLabel(label3);

            InterCodes codelabel3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            codelabel3->prev = codelabel3->next = codelabel3;
            codelabel3->code = (InterCode)malloc(sizeof(struct InterCode_));
            codelabel3->code->kind = LABEL_CODE;
            codelabel3->code->u.labelcode.label = copyLabel(label3);
            
            bindCode(code1, codelabel1);
            bindCode(code1, code2);
            bindCode(code1, gotolabel);
            bindCode(code1, codelabel2);
            bindCode(code1, code3);
            bindCode(code1, codelabel3);
            return code1;
        }
    }
    if( strcmp( Stmt->firstChild->token, "WHILE") == 0 ){
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();

        InterCodes code1 = translate_Cond(Stmt->firstChild->nextSibling->nextSibling, label2, label3, sym_table);
        InterCodes code2 = translate_Stmt(Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, sym_table);

        InterCodes codelabel1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        codelabel1->prev = codelabel1->next = codelabel1;
        codelabel1->code = (InterCode)malloc(sizeof(struct InterCode_));
        codelabel1->code->kind = LABEL_CODE;
        codelabel1->code->u.labelcode.label = copyLabel(label1);
        
        InterCodes codelabel2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        codelabel2->prev = codelabel2->next = codelabel2;
        codelabel2->code = (InterCode)malloc(sizeof(struct InterCode_));
        codelabel2->code->kind = LABEL_CODE;
        codelabel2->code->u.labelcode.label = copyLabel(label2);

        InterCodes gotolabel = (InterCodes)malloc(sizeof(struct InterCodes_));
        gotolabel->next = gotolabel->prev = gotolabel;
        gotolabel->code = (InterCode)malloc(sizeof(struct InterCode_));
        gotolabel->code->kind = LABEL_GOTO;
        gotolabel->code->u.labelgoto.label_goto = copyLabel(label1);

        InterCodes codelabel3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        codelabel3->prev = codelabel3->next = codelabel3;
        codelabel3->code = (InterCode)malloc(sizeof(struct InterCode_));
        codelabel3->code->kind = LABEL_CODE;
        codelabel3->code->u.labelcode.label = copyLabel(label3);

        bindCode(codelabel1, code1);
        bindCode(codelabel1, codelabel2);
        bindCode(codelabel1, code2);
        bindCode(codelabel1, gotolabel);
        bindCode(codelabel1, codelabel3);
        return codelabel1;
    }
    return NULL;
}

InterCodes translate_FunDec(struct TreeNode* FunDec, SyntaxNode sym_table){
    struct TreeNode* ID = FunDec->firstChild;
    SyntaxNode function = lookupfunc(ID);
    InterCodes func = (InterCodes)malloc(sizeof(struct InterCodes_));
    func->prev = func->next = func;
    func->code = (InterCode)malloc(sizeof(struct InterCode_));
    func->code->kind = FUNCTION;
    func->code->u.function.name = malloc(32);
    memcpy(func->code->u.function.name, function->name+'\0', 32);
    return func;
}
/*
 * Intermediate representation output
 */

void AssignOutput(InterCodes itc){
    printf("AssignOutput entry\n");
    Operand right = itc->code->u.assign.right;
    Operand left = itc->code->u.assign.left;
    if( left->kind == VARIABLE ){
        fputs("v", fp);
        fprintf(fp, "%d", left->u.var_no);
    }
    else if( left->kind == TEMP) {
        fputs("t", fp);
        fprintf(fp, "%d", left->u.temp_no);
    }
    
    if( right->kind == VARIABLE ){
        fputs(" := v", fp);
        fprintf(fp, "%d\n", right->u.var_no);
    }
    else if( right->kind == CONSTANT ){
        fputs(" := #", fp);
        fprintf(fp, "%d\n", right->u.value);
    }
    else if( right->kind == TEMP ){
        fputs(" := t", fp);
        fprintf(fp, "%d\n", right->u.temp_no);
    }
}
void AddOutput(InterCodes itc){ 
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;
    fputs("t", fp);
    fprintf(fp, "%d := ", result->u.temp_no);
    if( op1->kind == TEMP )
        fprintf(fp, "t%d + ", op1->u.temp_no);
    else if ( op1->kind == CONSTANT )
        fprintf(fp, "#%d + ", op1->u.value);

    if( op2->kind == TEMP )
        fprintf(fp, "t%d\n", op2->u.temp_no);
    else if ( op2->kind == CONSTANT )
        fprintf(fp, "#%d\n", op2->u.value);
}
void SubOutput(InterCodes itc){
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;
    fputs("t", fp);
    fprintf(fp, "%d := ", result->u.temp_no);
    if( op1->kind == TEMP )
        fprintf(fp, "t%d - ", op1->u.temp_no);
    else if ( op1->kind == CONSTANT )
        fprintf(fp, "#%d - ", op1->u.value);

    if( op2->kind == TEMP )
        fprintf(fp, "t%d\n", op2->u.temp_no);
    else if ( op2->kind == CONSTANT )
        fprintf(fp, "#%d\n", op2->u.value);
}
void MulOutput(InterCodes itc){
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;
    fputs("t", fp);
    fprintf(fp, "%d := ", result->u.temp_no);
    if( op1->kind == TEMP )
        fprintf(fp, "t%d * ", op1->u.temp_no);
    else if ( op1->kind == CONSTANT )
        fprintf(fp, "#%d * ", op1->u.value);

    if( op2->kind == TEMP )
        fprintf(fp, "t%d\n", op2->u.temp_no);
    else if ( op2->kind == CONSTANT )
        fprintf(fp, "#%d\n", op2->u.value);
}
void DivOutput(InterCodes itc){
    Operand result = itc->code->u.binop.result;
    Operand op1 = itc->code->u.binop.op1;
    Operand op2 = itc->code->u.binop.op2;
    fputs("t", fp);
    fprintf(fp, "%d := ", result->u.temp_no);
    if( op1->kind == TEMP )
        fprintf(fp, "t%d / ", op1->u.temp_no);
    else if ( op1->kind == CONSTANT )
        fprintf(fp, "#%d / ", op1->u.value);

    if( op2->kind == TEMP )
        fprintf(fp, "t%d\n", op2->u.temp_no);
    else if ( op2->kind == CONSTANT )
        fprintf(fp, "#%d\n", op2->u.value);
}
void LabelCodeOutput(InterCodes itc){
    Operand label = itc->code->u.labelcode.label;
    fputs("LABEL label", fp);
    fprintf(fp,"%d :\n", label->u.label_no);
}
void LabelTrueOutput(InterCodes itc){
    Operand t1 = itc->code->u.labeltrue.t1;
    Operand op = itc->code->u.labeltrue.op;
    Operand t2 = itc->code->u.labeltrue.t2;
    Operand label_true = itc->code->u.labeltrue.label_true;

    fputs("IF t", fp);
    fprintf(fp, "%d", t1->u.temp_no);
    if( op->u.op_no == 1 )
        fputs(" > ", fp);
    else if( op->u.op_no == 2)
        fputs(" < ", fp);
    else if( op->u.op_no == 3)
        fputs(" >= ", fp);
    else if( op->u.op_no == 4)
        fputs(" <= ", fp);
    else if( op->u.op_no == 5)
        fputs(" == ", fp);
    else if( op->u.op_no == 6)
        fputs(" != ", fp);

    if( t2->kind == TEMP )
        fprintf(fp, "t%d ", t2->u.temp_no);
    else if( t2->kind == CONSTANT )
        fprintf(fp, "#%d ", t2->u.value);

    fprintf(fp, "GOTO label%d\n", label_true->u.label_no);
}
void LabelGotoOutput(InterCodes itc){
    Operand label = itc->code->u.labelgoto.label_goto;
    fprintf(fp, "GOTO label%d\n", label->u.label_no);
}
void ReturnOutput(InterCodes itc){
    Operand op = itc->code->u.returnop.op;
    fputs("RETURN t", fp);
    fprintf(fp, "%d\n", op->u.temp_no);
}
void ReadOutput(InterCodes itc){
    Operand place = itc->code->u.read.place;
    fprintf(fp, "READ t%d\n", place->u.temp_no);
}
void CallfuncOutput(InterCodes itc){
    Operand place = itc->code->u.callfunc.place;
    fprintf(fp, "t%d := CALL ", place->u.temp_no);
    fprintf(fp, "%s\n", itc->code->u.callfunc.name);
}
void WritefuncOutput(InterCodes itc){
    Operand arg = itc->code->u.write.arg;
    fprintf(fp, "WRITE t%d\n", arg->u.temp_no);
}
void ArgOutput(InterCodes itc) {
    Operand arg = itc->code->u.arg.arg;
    fprintf(fp, "ARG t%d\n", arg->u.temp_no);
}
void FunctionOutput(InterCodes itc){
    fprintf(fp, "FUNCTION %s :\n", itc->code->u.function.name);
}
void IROutput()
{
   //init();
    fp = fopen("output.ir", "w+");   
    InterCodes itr = interCodes;
    while( itr != NULL ) {
        if( itr->code->kind == ASSIGN )
            AssignOutput(itr);
        else if( itr->code->kind == ADD )
            AddOutput(itr);
        else if( itr->code->kind == SUB )
            SubOutput(itr);
        else if( itr->code->kind == MUL)
            MulOutput(itr);
        else if( itr->code->kind == DIV)
            DivOutput(itr);
        else if( itr->code->kind == RETURN)
            ReturnOutput(itr);
        else if( itr->code->kind == LABEL_CODE)
            LabelCodeOutput(itr);
        else if( itr->code->kind == LABEL_TRUE)
            LabelTrueOutput(itr);
        else if( itr->code->kind == LABEL_GOTO)
            LabelGotoOutput(itr);
        else if( itr->code->kind == READ)
            ReadOutput(itr);
        else if( itr->code->kind == CALLFUNC)
            CallfuncOutput(itr);
        else if (itr->code->kind == WRITE)
            WritefuncOutput(itr);
        else if (itr->code->kind == ARG)
            ArgOutput(itr);
        else if (itr->code->kind == FUNCTION)
            FunctionOutput(itr);
        itr = itr->next;
        if( itr == interCodes )
            itr = NULL;
    }
    fclose(fp);
}
void IRTraverse(struct TreeNode* head)
{
    struct TreeNode *child;
    int i;

    if(head->token != NULL){
        if( strcmp(head->token, "FunDec") == 0 ){
            if( interCodes == NULL ){
                interCodes = translate_FunDec(head, syntax_table);
            }
            else{
                InterCodes code2 = translate_FunDec(head, syntax_table);
                bindCode(interCodes, code2);
            }
        }
        if( strcmp(head->token, "Stmt") == 0 ){
            printf("Stmt\n");
            if( interCodes == NULL ){
                printf("stmt null\n");
                interCodes = translate_Stmt(head, syntax_table);
            }
            else{
                printf("stmt bind\n");
                InterCodes code2 = translate_Stmt(head, syntax_table);
                bindCode(interCodes, code2);
            }

            IRTraverse(head->nextSibling);
            return ;
        }
        /*
        if( strcmp(head->token, "Exp") == 0) {
            printf("head->token Exp\n");
            Operand place = new_temp();
            // add the new intermedia code at the tail of the interCodes
            
            if( interCodes == NULL )
                interCodes = translate_Exp(head, syntax_table, place);
            else {
                InterCodes code2 = translate_Exp(head, syntax_table, place);
                bindCode(interCodes, code2);
            }
            return ;
        }
        */

        if(head->firstChild != NULL) {
            IRTraverse(head->firstChild);
        } 
        if(head->nextSibling == NULL){
        }
        else{
            IRTraverse(head->nextSibling);
        }
    }
    else{
        if( head->nextSibling != NULL)
            IRTraverse(head->nextSibling);
    }
}

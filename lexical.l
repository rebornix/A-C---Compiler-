%{
#include "syntax.tab.h"
struct TreeNode initial(int lineno, char* value, char* token);
%}
digit   [0-9]
letter  [a-zA-Z]
delim   [ \t\n]
ws      {delim}+
INT     ([1-9]{digit}*)|[0]
INT_8   [0]([0-7]+)
INT_16  [0](("x")|("X"))([0-9A-Fa-f]+)
FLOAT   ({digit}(\.{digit})?)((("e")|("E"))[+-]?{digit})?
Identity      {letter}({letter}|{digit}){0,9}
int yylineno = 1;
%%

[ \t] {}
\n  { yylineno++; }

";"     {yylval.node = initial((int)yylineno, (char*)yytext, "SEMI"); return SEMI; }
","     { yylval.node = initial((int)yylineno, (char*)yytext, "COMMA");return COMMA; }
"="     { yylval.node = initial((int)yylineno, (char*)yytext, "ASSIGNOP");return ASSIGNOP; }
"<"  | 
">"  |
">=" |
"<=" |
"==" |
"!="    { yylval.node = initial((int)yylineno, (char*)yytext, "RELOP");return RELOP; }
"+"     {yylval.node = initial((int)yylineno, (char*)yytext, "PLUS");
            return PLUS;
        }
"-"     { yylval.node = initial((int)yylineno, (char*)yytext, "MINUS");
            return MINUS;
        }
"*"     { yylval.node = initial((int)yylineno, (char*)yytext, "STAR");
            return STAR;
        }
"/"     { yylval.node = initial((int)yylineno, (char*)yytext, "DIV");
            return DIV;
        }
"&&"    {yylval.node = initial((int)yylineno, (char*)yytext, "AND"); return AND;  }
"||"    { yylval.node = initial((int)yylineno, (char*)yytext, "OR"); return OR;}
"."     { yylval.node = initial((int)yylineno, (char*)yytext, "DOT");return DOT; }
"!"     { yylval.node = initial((int)yylineno, (char*)yytext, "NOT");return NOT; }
{INT_16}   {  
            char *value = yytext;
            int iVal = 0;
            int length = sizeof(yytext);
            int i;
            value = value + 2;
            for(i = 0; i < length - 2; ++i){
                int count;
                int sum = 0;
                if(*value >= 48 && *value <= 57)
                    sum = *value - 48;
                if(*value >= 61 && *value <= 102)
                    sum = *value - 87;
                if(*value >= 65 && *value <= 70)
                    sum = *value - 55;
                value++;
                for(count = 0; count != length - i - 3; ++count)
                    sum = sum * 16;
                iVal = iVal + sum;
            }
            yylval.node = initial((int)yylineno, (char*)yytext, "INT");
            yylval.node.ival = iVal;
            return INT;
        }
{INT_8} {
            char *value = yytext;
            int iVal = 0;
            int length = sizeof(yytext);
            int i;
            for(i = 0; i < length - 1; ++i){
                int count;
                int sum = *value++ - 48;
                for(count = 0; count != length - i - 2; ++count)
                    sum = sum * 8;
                iVal = iVal + sum;
            }
            yylval.node = initial((int)yylineno, (char*)yytext, "INT");
            yylval.node.ival = iVal;
            return INT;
        }
{INT}   { yylval.node = initial((int)yylineno, (char*)yytext, "INT");
            yylval.node.ival = atoi(yytext);
            return INT;
        }
{FLOAT} { 
            yylval.node = initial((int)yylineno, (char*)yytext, "FLOAT");

            char temp[128];
            float sum = 0.0, q = 1.0;
            float exp;
            int num, j, pos= 0;
            char *p = yytext;
            while( *p && *p != 'E' && *p != 'e')
            {
                temp[pos] = *p;
                ++pos;
                ++p;
            }
            temp[pos] = '\0';
            sum = atof(temp);
            if(*p)
            {
                p++;
                memset(temp, 0, 128);
                pos = 0;
                if(*p == '-') {
                    exp = 1.0 / 10.0;
                    p++;
                }
                else if( *p == '+'){
                    exp = 10.0;
                    p++;
                }
                else
                    exp = 10.0;

                while(*p)
                {
                    temp[pos] = *p;
                    ++pos;
                    ++p;
                }
                num = atoi(temp);
                for( j = 0; j < num; j++)
                    q *= exp;
            }
            yylval.node.dval = sum * q;
            return FLOAT;
        }
"int"   { yylval.node = initial((int)yylineno, (char*)yytext, "TYPE"); return TYPE; }
"float" { yylval.node = initial((int)yylineno, (char*)yytext, "TYPE"); return TYPE; }
"("     { yylval.node = initial((int)yylineno, (char*)yytext, "LP");return LP; }
")"     { yylval.node = initial((int)yylineno, (char*)yytext, "RP");return RP; }
"["     { yylval.node = initial((int)yylineno, (char*)yytext, "LB");return LB; }
"]"     { yylval.node = initial((int)yylineno, (char*)yytext, "RB");return RB; }
"{"     { yylval.node = initial((int)yylineno, (char*)yytext, "LC");return LC; }
"}"     { yylval.node = initial((int)yylineno, (char*)yytext, "RC");return RC; }
"struct"  { yylval.node = initial((int)yylineno, (char*)yytext, "STRUCT");return STRUCT; }
"return"  { yylval.node = initial((int)yylineno, (char*)yytext, "RETURN");return RETURN; }
"if"      { yylval.node = initial((int)yylineno, (char*)yytext, "IF");return IF; }
"else"    { yylval.node = initial((int)yylineno, (char*)yytext, "ELSE");return ELSE; }
"while"   { yylval.node = initial((int)yylineno, (char*)yytext, "WHILE");return WHILE; }
{Identity}    { yylval.node = initial((int)yylineno, (char*)yytext, "ID");
                return ID; }
"//"    {
            char c = input();
            while(c != '\n' && c != EOF) c = input();
        }   
"/*"    {
            int succFinish = 0;
            char c = input();
            while(c != EOF) {
                if( c == '/' ){
                    char next = input();
                    if( next != EOF && next == '*'){
                        printf("Error type 3 at line: %d Mysterious charater: %c\n", yylineno, c);
                        errorJudge = 1;
                    }

                }
                if( c == '*' ){
                    char next = input();
                    if( next != EOF && next == '/'){
                        succFinish = 1;
                        break;
                    }
                }
                c = input();
            }
            //errorJudge = 1;
            if( succFinish != 1)
                 printf("Error type 3 at line %d \n", yylineno);
        }
.       { errorJudge = 1;printf("Error type 1 at line: %d Mysterious charater: %s\n", yylineno, yytext);  } 

%%
struct TreeNode 
initial(int lineno, char* value, char* token)
{
    struct TreeNode node;
    if( token != "INT" || token != "FLOAT") {
        node.value = malloc(32);
        memcpy(node.value, value +'\0', 32); // \0 is a must, because I use memcpy instead of strcpy
    }
    node.token = token;
    node.nextSibling = NULL;
    node.firstChild = NULL;
    node.lineno = lineno;
    return node;
}

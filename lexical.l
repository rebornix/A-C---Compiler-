%{
#include "syntax.tab.h"

struct TreeNode initial(int lineno, char* value, char* token);
%}
digit   [0-9]
letter  [a-zA-Z]
delim   [ \t\n]
ws      {delim}+
INT     -?{digit}+
FLOAT   -?(({digit}+)|({digit}*\.{digit}+))
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
{INT}   { yylval.node = initial((int)yylineno, (char*)yytext, "INT");
            return INT;
        }
{FLOAT} { yylval.node = initial((int)yylineno, (char*)yytext, "FLOAT");
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
"Else"    { yylval.node = initial((int)yylineno, (char*)yytext, "ELSE");return ELSE; }
"while"   { yylval.node = initial((int)yylineno, (char*)yytext, "WHILE");return WHILE; }
{Identity}    { yylval.node = initial((int)yylineno, (char*)yytext, "ID");return ID; }
.       { printf("Error type 1 at line: %d Mysterious charater: %s\n", yylineno, yytext);  } 

%%
struct TreeNode 
initial(int lineno, char* value, char* token)
{
    struct TreeNode node;
    node.value = value;
    node.token = token;
    node.nextSibling = NULL;
    node.firstChild = NULL;
    node.lineno = lineno;
    return node;
}

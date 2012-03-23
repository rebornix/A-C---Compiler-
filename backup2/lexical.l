%{
#include "syntax.tab.h"

struct TreeNode initial(int lineno, char* name);
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

";"     {yylval.node = initial((int)yylineno, (char*)yytext); return SEMI; }
","     { yylval.node = initial((int)yylineno, (char*)yytext);return COMMA; }
"="     { yylval.node = initial((int)yylineno, (char*)yytext);return ASSIGNOP; }
"<"  | 
">"  |
">=" |
"<=" |
"==" |
"!="    { yylval.node = initial((int)yylineno, (char*)yytext);return RELOP; }
"+"     {yylval.node = initial((int)yylineno, (char*)yytext);
            return PLUS;
        }
"-"     { yylval.node = initial((int)yylineno, (char*)yytext);
            return MINUS;
        }
"*"     { yylval.node = initial((int)yylineno, (char*)yytext);
            return STAR;
        }
"/"     { yylval.node = initial((int)yylineno, (char*)yytext);
            return DIV;
        }
"&&"    {yylval.node = initial((int)yylineno, (char*)yytext); return AND;  }
"||"    { yylval.node = initial((int)yylineno, (char*)yytext); }
"."     { yylval.node = initial((int)yylineno, (char*)yytext);return DOT; }
"!"     { yylval.node = initial((int)yylineno, (char*)yytext);return NOT; }
{INT}   { yylval.node = initial((int)yylineno, (char*)yytext);
            return INT;
        }
{FLOAT} { yylval.node = initial((int)yylineno, (char*)yytext);
            return FLOAT;
        }
"int"   {  yylval.node = initial((int)yylineno, (char*)yytext); return TYPE; }
"float" { yylval.node = initial((int)yylineno, (char*)yytext); return TYPE; }
"("     { yylval.node = initial((int)yylineno, (char*)yytext);return LP; }
")"     { yylval.node = initial((int)yylineno, (char*)yytext);return RP; }
"["     { yylval.node = initial((int)yylineno, (char*)yytext);return LB; }
"]"     { yylval.node = initial((int)yylineno, (char*)yytext);return RB; }
"{"     { yylval.node = initial((int)yylineno, (char*)yytext);return LC; }
"}"     { yylval.node = initial((int)yylineno, (char*)yytext);return RC; }
"struct"  { yylval.node = initial((int)yylineno, (char*)yytext);return STRUCT; }
"return"  { yylval.node = initial((int)yylineno, (char*)yytext);return RETURN; }
"if"      { yylval.node = initial((int)yylineno, (char*)yytext);return IF; }
"Else"    { yylval.node = initial((int)yylineno, (char*)yytext);return ELSE; }
"while"   { yylval.node = initial((int)yylineno, (char*)yytext);return WHILE; }
{Identity}    { yylval.node = initial((int)yylineno, (char*)yytext);return ID; }
.       { printf("Error ID at line: %d Mysterious charater: %s\n", yylineno, yytext);  } 

%%
struct TreeNode 
initial(int lineno, char* name)
{
    struct TreeNode node;
    node.lineno = lineno;
    node.name = name;
    return node;
}

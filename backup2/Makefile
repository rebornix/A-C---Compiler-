run:parser
	./parser source3.c
parser: syntax.tab.c
	gcc syntax.tab.c -lfl -ly -o parser
syntax.tab.c: syntax.y
	bison -d syntax.y
lex.yy.c: lexical.l
	flex lexical.l

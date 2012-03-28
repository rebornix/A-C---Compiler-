run: parser
	./parser test/test2.cmm
parser: syntax.tab.c lex.yy.c
	gcc syntax.tab.c -lfl -ly -o parser
syntax.tab.c: syntax.y
	bison -d syntax.y
lex.yy.c: lexical.l
	flex lexical.l
clean: 
	rm lex.yy.c  syntax.tab.c parser syntax.tab.h

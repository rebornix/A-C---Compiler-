run: parser
	./parser in
parser: syntax.tab.c lex.yy.c stable.c judge.c grammer.c semantic.c IR.c DR.c
	gcc syntax.tab.c stable.c judge.c grammer.c semantic.c IR.c DR.c -lfl -ly -o parser
syntax.tab.c: syntax.y 
	bison -d -v syntax.y 
lex.yy.c: lexical.l
	flex lexical.l
clean: 
	rm lex.yy.c  syntax.tab.c parser syntax.tab.h

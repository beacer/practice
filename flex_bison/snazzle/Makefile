all: snazzle

snazzle.tab.c snazzle.tab.h: snazzle.y
	bison -d snazzle.y

lex.yy.c: snazzle.l snazzle.tab.h
	flex snazzle.l

snazzle: lex.yy.c snazzle.tab.c snazzle.tab.h
	gcc snazzle.tab.c lex.yy.c -lfl -o snazzle

clean:
	-@rm snazzle.tab.c snazzle.tab.h lex.yy.c snazzle

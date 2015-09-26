fb1-5: fb1-5.l fb1-5.y
	bison -d fb1-5.y
	flex fb1-5.l
	cc -o $@ fb1-5.tab.c lex.yy.c -lfl

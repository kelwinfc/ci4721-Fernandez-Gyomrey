LEX    = flex
BISON  = bison
BFLAGS = --defines=y.tab.h --output=y.tab.c

test1:	lex.yy.c y.tab.c
		g++ $^ -o $@

y.tab.c:	parser.y
		bison -dy $<

lex.yy.c:	lexer.l
		flex $<

clean:
		rm test1 y.tab.h lex.yy.c y.tab.c

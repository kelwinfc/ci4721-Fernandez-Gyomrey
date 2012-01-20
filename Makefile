LEX    = flex
BISON  = bison
BFLAGS = --defines=y.tab.h --output=y.tab.c

all: lexer

lexer:	lex.yy.c y.tab.c
		gcc $^ -o $@ -lm -ly

y.tab.c:	parser.y
		bison -dy $<

lex.yy.c:	lexer.l
		flex $<

clean:
		rm lexer y.tab.h lex.yy.c y.tab.c

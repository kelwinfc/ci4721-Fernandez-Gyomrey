LEX    = flex
BISON  = bison
BFLAGS = --defines=y.tab.h --output=y.tab.c
FILES  = token

all: lexer y.tab.cc lex.yy.cc

lexer:	lex.yy.cc y.tab.cc $(FILES:%=%.h)
		g++ $^ -o $@ -lm -ly

y.tab.cc:	parser.y $(FILES:%=%.h)
		bison --skeleton="lalr1.cc" -dy $<

lex.yy.cc:	lexer.l $(FILES:%=%.h)
		flex -+ $<

clean:
		rm lexer y.tab.h lex.yy.cc y.tab.c

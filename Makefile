LEX    = flex
BISON  = bison
BFLAGS = --defines=y.tab.h --output=y.tab.c

GCC=g++
GCC_FLAGS= -Ilib -Wall -O3

FILES=token

DEP_token=
DEP_AST=

all: test1

test1:	lex.yy.c y.tab.c $(FILES:%=bin/%.o)
		g++ $^ -o $@

y.tab.c:	src/parser.y $(FILES:%=bin/%.o)
		bison -dy $<

lex.yy.c:	src/lexer.l $(FILES:%=bin/%.o) $(FILES:%=lib/%.h)
		flex $< 

#General rule for compiling
bin/%.o: src/%.cpp lib/%.h
	$(GCC) $(GCC_FLAGS) -c $< -o $@

bin/token.o: $(DEP_token:%=src/%.cpp) $(DEP_token:%=lib/%.h)

clean:
		rm test1 y.tab.h lex.yy.c y.tab.c

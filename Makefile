LEX    = flex
BISON  = bison
BFLAGS = --defines=y.tab.h --output=y.tab.c

GCC=g++
GCC_FLAGS= -Ilib -Wall -O3

FILES=token\
      AST\
      utils

DEP_AST=token utils
DEP_token=utils

all: bla

bla:	lex.yy.c y.tab.c $(FILES:%=bin/%.o)
		g++ $^ -o $@

y.tab.c:	src/parser.y $(FILES:%=bin/%.o)
		bison -dy $<

lex.yy.c:	src/lexer.l $(FILES:%=bin/%.o) $(FILES:%=lib/%.h)
		flex $< 

#General rule for compiling
bin/%.o: src/%.cpp lib/%.h
	$(GCC) $(GCC_FLAGS) -c $< -o $@

bin/AST.o: $(DEP_AST:%=src/%.cpp) $(DEP_AST:%=lib/%.h)
bin/token.o: $(DEP_token:%=src/%.cpp) $(DEP_token:%=lib/%.h)
bin/utils.o: $(DEP_utils:%=src/%.cpp) $(DEP_utils:%=lib/%.h)

doc:
	markdown2pdf doc/especificacion.mdwn -o doc/especificacion.pdf
clean:
		rm -f bin/*.o *~ bin/*~ src/*~ lib/*~ test/*~ bla src/y.tab.h src/lex.yy.c src/y.tab.c

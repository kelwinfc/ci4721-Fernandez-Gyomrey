LEX    = flex
BISON  = bison
BFLAGS = --defines=y.tab.h --output=y.tab.c

GCC=g++
GCC_FLAGS= -Ilib -Wall -O3

FILES=token\
      AST\
      utils\
      symbol\
      symbol_table\
      type_descriptor\
      type_table\
      llog

DEP_AST=token utils symbol symbol_table llog
DEP_token=utils llog
DEP_symbol_table=symbol
DEP_type_descriptor=utils symbol symbol_table
DEP_type_table=utils symbol symbol_table type_descriptor
all: bla

bla: lex.yy.c y.tab.c $(FILES:%=bin/%.o)
	g++ $^ -o $@

y.tab.c: src/parser.y $(FILES:%=bin/%.o)
	bison -dy $<

lex.yy.c: src/lexer.l $(FILES:%=bin/%.o) $(FILES:%=lib/%.h)
	flex $< 

#General rule for compiling
bin/%.o: src/%.cpp lib/%.h
	$(GCC) $(GCC_FLAGS) -c $< -o $@

bin/AST.o: $(DEP_AST:%=src/%.cpp) $(DEP_AST:%=lib/%.h)
bin/token.o: $(DEP_token:%=src/%.cpp) $(DEP_token:%=lib/%.h)
bin/utils.o: $(DEP_utils:%=src/%.cpp) $(DEP_utils:%=lib/%.h)
bin/symbol.o: $(DEP_symbol:%=src/%.cpp) $(DEP_symbol:%=lib/%.h)
bin/symbol_table.o: $(DEP_symbol_table:%=src/%.cpp) $(DEP_symbol_table:%=lib/%.h)
bin/llog.o: $(DEP_llog:%=src/%.cpp) $(DEP_llog:%=lib/%.h)
bin/type_descriptor.o: $(DEP_type_descriptor:%=src/%.cpp) $(DEP_type_descriptor:%=lib/%.h)
bin/type_table.o: $(DEP_type_table:%=src/%.cpp) $(DEP_type_table:%=lib/%.h)

doc:
	markdown2pdf doc/especificacion_primera_entrega.mdwn -o doc/especificacion_primera_entrega.pdf
clean:
		rm -f bin/*.o *~ bin/*~ src/*~ lib/*~ test/*~ bla y.tab.h lex.yy.c y.tab.c src/AST/*~


GCC=g++
#GCC_DEBUG=-ggdb
GCC_FLAGS=-Ilib -Wall -O3 $(GCC_DEBUG)
GCC_MAIN_FLAGS=-Ilib -O3 $(GCC_DEBUG)

FILES=token\
      AST\
      utils\
      symbol\
      symbol_table\
      type_descriptor\
      type_table\
      llog\
      block\
      inst\
      optimizer\
      string_table\
      main\
      Tac

DEP_AST=llog token utils symbol symbol_table string_table Tac
DEP_token=utils llog
DEP_symbol_table=symbol string_table
DEP_type_descriptor=utils symbol symbol_table
DEP_type_table=utils symbol symbol_table type_descriptor
DEP_block=inst
DEP_optimizer=inst block
DEP_main=AST symbol symbol_table llog type_table optimizer string_table Tac
DEP_Tac=block inst optimizer

all: bla

bla: $(FILES:%=bin/%.o)
	$(GCC) $(GCC_DEBUG) $^ -o $@

#main tiene flags distintos
bin/main.o: src/main.cpp lib/main.h
	$(GCC) $(GCC_MAIN_FLAGS) -c src/main.cpp -o bin/main.o

#General rule for compiling
bin/%.o: src/%.cpp lib/%.h
	$(GCC) $(GCC_FLAGS) -c $< -o $@

bin/main.o: $(DEP_main:%=src/%.cpp) $(DEP_main:%=lib/%.h) lex.yy.c y.tab.c
bin/AST.o: $(DEP_AST:%=src/%.cpp) $(DEP_AST:%=lib/%.h)
bin/token.o: $(DEP_token:%=src/%.cpp) $(DEP_token:%=lib/%.h)
bin/utils.o: $(DEP_utils:%=src/%.cpp) $(DEP_utils:%=lib/%.h)
bin/symbol.o: $(DEP_symbol:%=src/%.cpp) $(DEP_symbol:%=lib/%.h)
bin/symbol_table.o: $(DEP_symbol_table:%=src/%.cpp) $(DEP_symbol_table:%=lib/%.h)
bin/llog.o: $(DEP_llog:%=src/%.cpp) $(DEP_llog:%=lib/%.h)
bin/type_descriptor.o: $(DEP_type_descriptor:%=src/%.cpp) $(DEP_type_descriptor:%=lib/%.h)
bin/type_table.o: $(DEP_type_table:%=src/%.cpp) $(DEP_type_table:%=lib/%.h)
bin/Tac.o: $(DEP_Tac:%=src/%.cpp) $(DEP_Tac:%=lib/%.h)
bin/block.o: $(DEP_block:%=src/%.cpp) $(DEP_block:%=lib/%.h)
bin/inst.o: $(DEP_inst:%=src/%.cpp) $(DEP_inst:%=lib/%.h)
bin/optimizer.o: $(DEP_optimizer:%=src/%.cpp) $(DEP_optimizer:%=lib/%.h)
bin/string_table.o: $(DEP_string_table:%=src/%.cpp) $(DEP_string_table:%=lib/%.h)

y.tab.c: src/parser.y
	bison -v -dy $<

lex.yy.c: src/lexer.l
	flex $< 

doc:
	markdown2pdf doc/especificacion_primera_entrega.mdwn -o doc/especificacion_primera_entrega.pdf
clean:
		rm -f doc/*~ bin/*.o *~ bin/*~ src/*~ lib/*~ test/*~ bla y.tab.h lex.yy.c y.tab.c src/AST/*~ y.output

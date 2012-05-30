#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

#include <map>
#include <string>
#include <typeinfo>
#include <vector>
#include <iostream>
#include <cstdio>
#include "llog.h"
#include "symbol.h"

extern llog* logger;

using namespace std;

extern bool error;

class symbol_table {
    private:

        symbol_table* parent;
        unsigned int counter;

    public:
        unsigned int accumulated_offset;
        
        map<string, symbol*> table;
        
        symbol_table();
        
        ~symbol_table(){
            /* Verificar si hay simbolos sin ser utilizados en el bloque */
            map<string, symbol*>::iterator it;
            for (it = table.begin(); it != table.end(); ++it){
                symbol* sym = it->second;
                if ( sym->unused && !sym->is_function ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Identificador sin utilizar '%s'.",
                             sym->getName().c_str());
                    logger->warning(sym->getLine(), sym->getColumn(), e);
                }
            }
            printf("eliminando tabla %p\n", this);
        }

        symbol_table(symbol_table* p);
        
        symbol_table getParent();
        
        symbol_table* new_son();
        
        symbol* lookup(string name, int* level=0);
        
        void accumulate_offset(int width, int alignment);
        
        void insert(symbol* s);
};

#endif
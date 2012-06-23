#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

#include <map>
#include <string>
#include <typeinfo>
#include <vector>
#include <iostream>
#include "llog.h"
#include "symbol.h"
#include "string_table.h"

extern llog* logger;

using namespace std;

extern bool error;

class symbol_table {
    private:

        symbol_table* parent;
        unsigned int counter;

    public:
        unsigned int accumulated_offset;

        string_table* strings;
        
        map<string, symbol*> table;
        
        symbol_table(string_table* s);
        
        ~symbol_table(){
            /* Verificar si hay simbolos sin ser utilizados en el bloque */
            map<string, symbol*>::iterator it;
            for (it = table.begin(); it != table.end(); ++it){
                symbol* sym = it->second;
                if ( sym->unused && !sym->is_function ){
                    logger->buf << "Identificador sin utilizar '" << sym->getName() << "'.";
                    logger->warning(sym->getLine(), sym->getColumn());
                }
            }
        }

        symbol_table(symbol_table* p, string_table* s);
        
        symbol_table getParent();
        
        symbol_table* new_son();
        
        symbol* lookup(string name, int* level=0);
        
        void accumulate_offset(int width, int alignment);
        
        void insert(symbol* s);
};

#endif
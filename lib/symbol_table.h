#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

#include<map>
#include<string>
#include "symbol.h"
#include <typeinfo>
#include <vector>

using namespace std;

extern bool error;

class symbol_table {
    private:

        symbol_table* parent;

    public:
        unsigned int accumulated_offset;
        
        map<string, symbol*> table;
        
        symbol_table();
        
        symbol_table(symbol_table* p);
        
        symbol_table getParent();
        
        symbol_table* new_son();
        
        symbol* lookup(string name, int* level=0);
        
        void accumulate_offset(int width, int alignment);
        
        void insert(symbol* s);
};

#endif
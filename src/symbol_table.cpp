#include "symbol_table.h"

symbol_table::symbol_table(){
    parent = 0;
}

symbol* symbol_table::lookup(string name){
    map<string, symbol*>::iterator iter;
    if ( table.find(name) != table.end() ){
        return table[name];
    }
    
    if (0 != parent){
        return (*parent).lookup(name);
    }

    return 0;
}

symbol_table symbol_table::getParent(){
    return *parent;
}

void symbol_table::insert(symbol* s){
    table[s->getName()] = s;
}

//TODO agregar tabla hija
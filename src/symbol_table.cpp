#include "symbol_table.h"

symbol_table::symbol_table(){
    parent = 0;
}

symbol_table::symbol_table(symbol_table* p){
    parent = p;
}

symbol* symbol_table::lookup(string name, int* level){
    map<string, symbol*>::iterator iter;
    if ( table.find(name) != table.end() ){
        if (level){
            *level = 0;
        }
        return table[name];
    }
    
    if (0 != parent){
        return (*parent).lookup(name, level);
    }
    
    return 0;
}

symbol_table symbol_table::getParent(){
    return *parent;
}

void symbol_table::insert(symbol* s){
    table[s->getName()] = s;
}

symbol_table* symbol_table::new_son(){
    return new symbol_table(this);
}
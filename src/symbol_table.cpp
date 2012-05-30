#include "symbol_table.h"

symbol_table::symbol_table(){
    accumulated_offset = 0;
    parent = 0;
}

symbol_table::symbol_table(symbol_table* p){
    parent = p;
    
    if ( p == 0 || p->parent == 0 ){
        accumulated_offset = 0;
    } else if ( p != 0 && p->parent != 0 ){
        accumulated_offset = p->accumulated_offset;
    }
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
        symbol* s = parent->lookup(name, level);
        
        if ( level){
            *level = *level + 1;
        }
        return s;
    }
    
    return 0;
}

symbol_table symbol_table::getParent(){
    return *parent;
}

void symbol_table::insert(symbol* s){
    table[s->getName()] = s;
    
    accumulate_offset( 0, s->alignment );
    s->offset = accumulated_offset;
    s->index = ++ counter;
    
    accumulate_offset( s->width, s->alignment );
}

symbol_table* symbol_table::new_son(){
    return new symbol_table(this);
}

void symbol_table::accumulate_offset(int width, int alignment){
    
    if ( alignment <= 0 ){
        return;
    }
    
    accumulated_offset += (alignment
                            - (accumulated_offset % alignment)) % alignment;
    
    accumulated_offset += width;
}

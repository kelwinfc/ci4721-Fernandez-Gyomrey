#ifndef __TYPE_DESCRIPTOR
#define __TYPE_DESCRIPTOR

#include "symbol_table.h"

class type_descriptor{
    
    public:
        string name;
        int width;
        int alignment;
    
        type_descriptor(){ width = 0; alignment = 1; }
        
        type_descriptor(string n, int w = 0, int a = 1);
};

class struct_descriptor : public type_descriptor {
    
    public:
        symbol_table* fields;

        struct_descriptor(string n, symbol_table* st);
};

#endif
#ifndef __TYPE_DESCRIPTOR
#define __TYPE_DESCRIPTOR

#include "symbol_table.h"
#include <stdio.h>

class type_descriptor{
    
    public:
        string name;
        int width;
        int alignment;
    
        type_descriptor(){ width = 0; alignment = 1; }
        
        type_descriptor(string n, int w = 0, int a = 1);
        
        virtual void print(FILE* fd);
};

class struct_type : public type_descriptor {
    
    public:
        symbol_table* fields;

        struct_type(string n, symbol_table* st);
        
        virtual void print(FILE* fd);
};

class array_descriptor : public type_descriptor {
    public:
        int lower_index;
        int upper_index;
        int num_elements;
        TYPE base;
        
        array_descriptor( type_descriptor* b, TYPE b_ind, int up, int low );
};

class pointer_descriptor : public type_descriptor {
    public:
        TYPE base;
        
        pointer_descriptor(TYPE b);
};

#endif

#ifndef __TYPE_DESCRIPTOR
#define __TYPE_DESCRIPTOR

#include "symbol_table.h"
#include "token.h"
#include "type_table.h"
#include <stdio.h>
#include <set>

class type_table;

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

        struct_type(){}

        struct_type(string n, symbol_table* st);

        virtual void print(FILE* fd);
};

class union_type : public struct_type {
    public:

        union_type(string n, symbol_table* st);

        virtual void print(FILE* fd);
};

class enum_type : public type_descriptor {
    public:
        int line;
        int column;
        set<string>* values;

        enum_type( tokenId* tk, set<string>* values );

        virtual void print(FILE* fd);
};

class array_descriptor : public type_descriptor {
    public:
        int lower_index;
        int upper_index;
        int num_elements;
        TYPE base;

        array_descriptor( type_descriptor* b, TYPE b_ind, int up, int low = 0 );
};

class pointer_descriptor : public type_descriptor {
    public:
        TYPE base;
        
        pointer_descriptor(TYPE b, string base_name);
};

#endif

#ifndef __TYPE_DESCRIPTOR
#define __TYPE_DESCRIPTOR

#include "symbol_table.h"
#include <stdio.h>
#include <map>
#include <vector>

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
        
        pointer_descriptor(TYPE b, string base_name);
};

class type_table{
    
    public :
        vector< type_descriptor* > types;
        map<string, int> names;
        
        type_table();
        
        bool has_type( string s );
        
        bool is_base( string s );
        
        bool is_base( int n );
        
        int index_of( string n );
        
        int add_type( type_descriptor* td );
        
        void add_type( string t );
        
        type_descriptor* get_type( string t );
        
        void add_alias( string a, string b );
};

#endif

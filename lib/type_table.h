#ifndef __TYPE_TABLE
#define __TYPE_TABLE

#include "type_descriptor.h"
#include <map>
#include <vector>
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
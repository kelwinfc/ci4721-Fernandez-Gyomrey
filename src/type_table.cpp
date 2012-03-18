#include "type_table.h"
#include <iostream>

using namespace std;

type_table::type_table(){
    add_type( new type_descriptor("_undefined") );
    add_type( new type_descriptor("_invalid") );
    add_type( new type_descriptor("none") );
    add_type( new type_descriptor("int", 4, 4) );
    add_type( new type_descriptor("float", 4, 4) );
    add_type( new type_descriptor("char", 1, 1) );
    add_type( new type_descriptor("boolean", 1, 1) );
    add_type( new type_descriptor("string", 4, 4) );
}
        
bool type_table::has_type( string s ){
    return names.find(s) != names.end();
}
        
bool type_table::is_base( string s ){
    return is_base( index_of(s) );
}
        
bool type_table::is_base( int n ){
    return INT <= n && n <= STRING;
}

int type_table::index_of( string s ){
    if ( names.find(s) == names.end() ){
        return UNDEFINED;
    }
    
    return names[s];
}

type_descriptor* type_table::get_type( string s ){
    if ( names.find(s) == names.end() ){
        return 0;
    }

    return types[ names[ s ] ];
}

int type_table::add_type( type_descriptor* td ){

    map<string, int>::iterator i = names.find( td->name );
    if ( i == names.end() ){
        names[ td->name ] = types.size();
        types.push_back( td );
        
        return types.size()-1;
    }
    
    return (*i).second;
}

void type_table::add_type( string t ){
    
    if ( names.find( t ) == names.end() ){
        names[ t ] = types.size();
        types.push_back( new type_descriptor(t) );
    }
}

void type_table::add_alias( string a, string b ){
    names[b] = index_of(a);
}
#include "type_descriptor.h"
#include <sstream>
#include <iostream>

extern type_table types;

type_descriptor::type_descriptor(string n, int w, int a){
    name = n;
    width = w;
    alignment = a;
}

struct_type::struct_type(string n, symbol_table* st){
    name = n;
    width = 0;
    alignment = 1;
    fields = st;
}

void type_descriptor::print(FILE* fd){
    fprintf(fd, "  * %s\n", name.c_str());
}

void struct_type::print(FILE* fd){
    fprintf(fd, "  * %s\n", name.c_str());
    fprintf(fd, "    ---\n");
    map<string, symbol*>::iterator it;
    for (it = fields->table.begin(); it != fields->table.end(); ++it){
        fprintf(fd, "    |   - %s %s\n", 
                types.types[(*it).second->getType()]->name.c_str(),
                (*it).first.c_str()
               );
    }
    if ( fields->table.begin() == fields->table.end() ){
        fprintf(fd, "    |\n");
    }
    fprintf(fd, "    ---\n");
}

array_descriptor::array_descriptor( type_descriptor* b, TYPE b_ind, int up,
                                    int low )
{
    stringstream ss;
    ss << "["
       << b->name
       << "("
       << low
       << ","
       << up
       << ")"
       << "]";
    ss >> name;
    
    width = b->width * ( up - low );
    alignment = b->alignment;
    
    lower_index = low;
    upper_index = up;
    num_elements = up - low;
    
    base = b_ind;
}

pointer_descriptor::pointer_descriptor(TYPE b_index, string base_name){
    base = b_index;
    
    stringstream ss;
    ss << "pointer_to_"
       << base_name;
    ss >> name;
    
    width = 4;
    alignment = 1;
}

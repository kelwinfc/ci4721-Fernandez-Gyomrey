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
    width = st->accumulated_offset;
    alignment = 4;
    fields = st;
}

union_type::union_type(string n, symbol_table* st){
    name = n;
    width = 0;
    alignment = 1;
    fields = st;
    
    map<string, symbol*>::iterator it;
    for (it = fields->table.begin(); it != fields->table.end(); ++it){
        (*it).second->offset = 0;
        width = max(width, types.types[(*it).second->getType()]->width );
        
        alignment = lcm(alignment,
                        types.types[(*it).second->getType()]->alignment );
    }
}

enum_type::enum_type( tokenId* tk, set<string>* v)
{
    width = 4;
    alignment = 4;
    line = tk->line;
    column = tk->column;
    name = tk->ident;
    values = v;

    delete tk;
}

array_descriptor::array_descriptor( type_descriptor* b, TYPE b_ind, int up,
                                    int low )
{
    stringstream ss;
    ss << "array"
       << "("
       << low
       << ","
       << up
       << ")_of_"
       << b->name;
    ss >> name;
    
    unsigned int base_width = b->width;
    base_width += (b->alignment - (base_width) % b->alignment ) % b->alignment;
    
    width = base_width * ( up - low );
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
    alignment = 4;
}

void type_descriptor::print(FILE* fd){
    fprintf(fd, "  * %s [size %d][alignment %d]\n",
            name.c_str(),
            width,
            alignment
           );
}

void struct_type::print(FILE* fd){
    fprintf(fd, "  * struct %s [size %d][alignment %d]\n", name.c_str(),
            width, alignment);
    fprintf(fd, "    ---\n");
    map<string, symbol*>::iterator it;
    for (it = fields->table.begin(); it != fields->table.end(); ++it){
        fprintf(fd, "    |   - %s %s [offset %d]\n", 
                types.types[(*it).second->getType()]->name.c_str(),
                (*it).first.c_str(),
                (*it).second->offset
               );
    }
    if ( fields->table.begin() == fields->table.end() ){
        fprintf(fd, "    |\n");
    }
    fprintf(fd, "    ---\n");
}

void union_type::print(FILE* fd){
    fprintf(fd, "  * union %s [size %d][alignment %d]\n", name.c_str(), 
            width, alignment);
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

void enum_type::print(FILE* fd){
    fprintf(fd, "  * enum %s [size %d][alignment %d]\n", 
            name.c_str(),
            width,
            alignment
           );
    fprintf(fd, "    ---\n");
    set<string>::iterator it;
    for (it = values->begin(); it != values->end(); ++it){
        fprintf(fd, "    |   - %s\n", (*it).c_str());
    }
    if ( values->begin() == values->end() ){
        fprintf(fd, "    |\n");
    }
    fprintf(fd, "    ---\n");
}
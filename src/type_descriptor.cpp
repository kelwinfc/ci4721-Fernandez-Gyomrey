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
    // entero de 4 bytes para indicar posición en uso actualmente
    width += 4;
}
// TODO no deberían recibirse tokens aquí.
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

void type_descriptor::dump(ostream &strm){
    strm << "  * " << name << " [size " << width
         << "][alignment " << alignment << "]" << endl;
}

void struct_type::dump(ostream &strm){
    strm << "  * struct " << name << " [size " << width
         << "][alignment " << alignment << "]" << endl
         << "    ---" << endl;
    map<string, symbol*>::iterator it;
    for (it = fields->table.begin(); it != fields->table.end(); ++it){
        strm << "    |   - " << types.types[(*it).second->getType()]->name
             << ' ' << (*it).first << " [offset " << (*it).second->offset << ']' << endl;
    }
    if ( fields->table.begin() == fields->table.end() ){
        strm << "    |" << endl;
    }
    strm << "    ---" << endl;
}

void union_type::dump(ostream &strm){
    strm << "  * union " << name << " [size " << width
         << "][alignment " << alignment << "]" << endl
         << "    ---" << endl;
    map<string, symbol*>::iterator it;
    for (it = fields->table.begin(); it != fields->table.end(); ++it){
        strm << "    |   - " << types.types[(*it).second->getType()]->name
             << ' ' << (*it).first << endl;
    }
    if ( fields->table.begin() == fields->table.end() ){
        strm << "    |" << endl;
    }
    strm << "    ---" << endl;
}

void enum_type::dump(ostream &strm){
    strm << "  * enum " << name << " [size " << width
         << "][alignment " << alignment << "]" << endl
         << "    ---" << endl;
    set<string>::iterator it;
    for (it = values->begin(); it != values->end(); ++it){
        strm << "    |   - " << (*it).c_str() << endl;
    }
    if ( values->begin() == values->end() ){
        strm << "    |" << endl;
    }
    strm << "    ---" << endl;
}
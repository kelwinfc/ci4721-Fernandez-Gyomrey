#include "type_descriptor.h"

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
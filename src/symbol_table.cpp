
#include "symbol_table.h"

symbol_table::symbol_table(){
    parent = 0;
}

symbol* symbol_table::lookup(string name){
    map<string, symbol*>::iterator iter;
    if ( table.find(name) != table.end() ){
        return table[name];
    }
    
    if (0 != parent){
        return (*parent).lookup(name);
    }

    return 0;
}

symbol_table symbol_table::getParent(){
    return *parent;
}

void symbol_table::insert(symbol* s){
    table[s->getName()] = s;
}

bool symbol_table::fill_with(AST_node* a){
    
    if ( typeid(*a) == typeid(AST_program) ){
        AST_program* p = (AST_program*)a;
        
        vector<AST_declaration*>::iterator it;
        
        /* Agregar las firmas de las funciones a la tabla
         * Esta pasada se realiza solo sobre la raiz del arbol
         */
        for (it = p->declarations.begin(); it != p->declarations.end(); ++it){
            AST_declaration* dec = *it;
            
            if ( typeid(*dec) == typeid(AST_function) ){

                symbol_function* function = 
                    new symbol_function( *(AST_function*)dec );
                this->insert(function);
                
            }
        }
        
        /* Llenar la tabla de simbolos con la declaracion de variables globales
         * y finalizar el llenado de la tabla con la informacion local de las
         * funciones
         */
        for (it = p->declarations.begin(); it != p->declarations.end(); ++it){
            AST_declaration* dec = *it;
            
            if ( typeid(*dec) == typeid(AST_function) ){
                
            }
        }
    }
    return true;
}

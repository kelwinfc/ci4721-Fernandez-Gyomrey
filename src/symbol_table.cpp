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

bool symbol_table::check(AST_node* a){
    
    return true;
    
    cout << "verificando\n";
    a->print(1);
    
    return true;
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
                
                // Agregar simbolo al nivel actual de la tabla de simbolos
                if ( lookup(dec->identifier->ident) ){
                    fprintf(stderr, "Funcion \"%s\" previamente definida.\n",
                            dec->identifier->ident
                           );
                    error = true;
                } else {
                    
                    symbol_function* function = 
                        new symbol_function( *(AST_function*)dec );
                    this->insert(function);
                }
            }
        }
        
        /* Llenar la tabla de simbolos con la declaracion de variables globales
         * y finalizar el llenado de la tabla con la informacion local de las
         * funciones
         */
        for (int i=p->declarations.size()-1; i >= 0; i--){
            AST_declaration* dec = p->declarations[i];
            
            if ( typeid(*dec) == typeid(AST_function) ){
                
            } else if ( typeid(*dec) == typeid(AST_variable_declaration) ){
                
                // Crear simbolo correspondiente a la variable global
                AST_variable_declaration* v = (AST_variable_declaration*)dec;
                symbol *var = new symbol(v->type->type(), v->identifier->ident);
                
                // Agregar simbolo al nivel actual de la tabla de simbolos
                if ( lookup(v->identifier->ident) ){
                    fprintf(stderr,
                            "Identificador \"%s\" previamente definido.\n",
                            v->identifier->ident
                           );
                    error = true;
                } else {
                    this->insert(var);
                }
                
                /* Verificar que los simbolos utilizados en su inicializacion
                 * esten previamente declarados
                 */
                if ( v->value ){
                    this->check(v->value);
                }
            }
        }
    }
    
    /* TODO: metodos que agreguen a la tabla de simbolos. Consejo: utilizar
     * metodos especificos para cada tipo, son mas faciles de depurar.
     * Pasar lo que se hizo anteriormente a este estilo.
     * 
     *  AST_expression
     *  AST_parenthesis
     *  AST_op
     *  AST_un_op
     *  AST_int
     *  AST_float
     *  AST_boolean
     *  AST_ident
     *  AST_char
     *  AST_parameters_list
     *  AST_function_call
     *  AST_block
     *  AST_function
     *  AST_function
     *  AST_assignment
     *  AST_return
     *  AST_conditional
     *  AST_loop
     *  AST_bounded_loop
     */ 
    return true;
}

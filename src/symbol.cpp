
#include "symbol.h"

string symbol::getName(){
    return name;
}

bool symbol::isConst(){
    return constant;
}

string symbol::getType(){
    return type;
}

int symbol::getLine(){
    return line;
}

int symbol::getColumn(){
    return column;
}

bool symbol::isInitialized(){
    return initialized;
}

symbol_function::symbol_function(AST_function& f){
    
    this->name = f.identifier;
    
    if ( f.type ){
        this->type = f.type;
    } else {
        this->type = (char*)"none";
    }
    
    this->params.clear();
    
    vector< pair< AST_arg_list::TYPE, string> >::iterator it;
    
    for( it = f.formal_parameters->args.begin();
         it != f.formal_parameters->args.end();
         ++it
       )
    {
        string type;
        switch ( it->first ) {
            case AST_arg_list::INT:
                type = "int";
            case AST_arg_list::FLOAT:
                type = "float";
            case AST_arg_list::CHAR:
                type = "char";
            case AST_arg_list::BOOLEAN:
                type = "boolean";
            case AST_arg_list::NONE:
                type = "none";
        }
        //TODO no funciona
        //symbol next_arg( type, it->second );
        //params.push_back( next_arg );
    }
}
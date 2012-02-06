
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
    
    vector< pair<tokenType*, tokenId*> >::iterator it;
    
    for( it = f.formal_parameters->args.begin();
         it != f.formal_parameters->args.end();
         ++it
       )
    {
        symbol next_arg( it->first->type(), it->second->ident );
        params.push_back( next_arg );
    }
}
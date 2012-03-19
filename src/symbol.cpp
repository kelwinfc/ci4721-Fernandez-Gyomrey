#include "symbol.h"

symbol::symbol(){}

symbol::symbol(string name, bool constant, TYPE type, int line,
               int column, bool initialized)
{
    //al construir se puede validar que no sea NONE!
    this->name = name;
    this->constant = constant;
    this->type = type;
    this->line = line;
    this->column = column;
    this->initialized = initialized;
    this->is_function = false;
    
    this->width = 0;
    this->alignment = 1;
    this->offset = 0;
}

string symbol::getName(){
    return name;
}

bool symbol::isConst(){
    return constant;
}

TYPE symbol::getType(){
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

symbol_function::symbol_function(){}

symbol_function::symbol_function(string name, int line, int column,
                                 vector<TYPE>& types
                                )
{
    this->name = name;
    this->type = NONE;
    this->constant = true;
    
    this->line = line;
    this->column = column;
    this->initialized = false;
    this->is_function = true;
    
    vector< TYPE >::iterator it;
    for ( it = types.begin(); it != types.end(); ++it ){
        this->params.push_back( *it );
    }
}

symbol_function::symbol_function(string name, TYPE type, int line, int column,
                                 vector<TYPE>& types
                                )
{
    this->name = name;
    this->constant = true;
    this->line = line;
    this->column = column;
    this->initialized = false;
    this->type = type;
    this->is_function = true;
    
    vector< TYPE >::iterator it;
    for ( it = types.begin(); it != types.end(); ++it ){
        this->params.push_back( *it );
    }
}

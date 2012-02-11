
#include "symbol.h"

symbol::symbol(){}

symbol::symbol(string name, bool constant, TYPE type, int line, int column, bool initialized){
    //al construir se puede validar que no sea NONE!
    this->name = name;
    this->constant = constant;
    this->type = type;
    this->line = line;
    this->column = column;
    this->initialized = initialized;
    this->is_function = false;
}

string symbol::getName(){
    return name;
}

bool symbol::isConst(){
    return constant;
}

symbol::TYPE symbol::getType(){
    return type;
}

string symbol::getTypeName(){
    switch (type){
        case INT:
            return "int";
        case FLOAT:
            return "float";
        case CHAR:
            return "char";
        case BOOLEAN:
            return "boolean";
        case NONE:
            return "none";
        case UNDEFINED:
            return "undefined";
        case INVALID:
            return "invalid";
    }
    return "";
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

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

symbol_int::symbol_int(string name, int value){
	this->name = name;
	this->value = value;
}

symbol_double::symbol_double(string name, double value){
	this->name = name;
	this->value = value;
}

symbol_char::symbol_char(string name, char value){
	this->name = name;
	this->value = value;
}

int symbol_int::getValue(){
	return value;
}

double symbol_double::getValue(){
	return value;
}

char symbol_char::getValue(){
	return value;
}
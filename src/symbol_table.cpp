
#include "symbol_table.h"

symbol_table::symbol_table(){
	parent = 0;
}

symbol* symbol_table::lookup(string name){
	map<string, symbol*>::iterator iter;
	for (iter = table.begin(); iter != table.end(); ++ iter) {
		if ( (*iter).first.compare(name) == 0 ){
			return (*iter).second;
		}
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
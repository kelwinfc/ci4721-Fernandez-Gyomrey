#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

#include<map>
#include<string>
#include "symbol.h"

using namespace std;

class symbol_table {
    private:

        map<string, symbol*> table;

		symbol_table* parent;

	public:

        symbol_table();

		symbol_table getParent();

        symbol* lookup(string name);

		void insert(symbol* s);
};

#endif
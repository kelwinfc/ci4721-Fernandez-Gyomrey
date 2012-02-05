#ifndef __SYMBOL
#define __SYMBOL

#include <string>
#include <vector>
#include "AST.h"

using namespace std;

class symbol{

    protected:

        string name;

        bool constant;

        string type;

        int line;

        int column;

        bool initialized;

    public:

        symbol(){}
        
        symbol(char* t, char* n){
            name = n;
            type = t;
        }
        
        string getName();

        bool isConst();

        string getType();

        int getLine();

        int getColumn();

        bool isInitialized();
};

class symbol_function : public symbol {
    public:
        
        vector<symbol> params;
        
        symbol_function(AST_function& f);
};

#endif
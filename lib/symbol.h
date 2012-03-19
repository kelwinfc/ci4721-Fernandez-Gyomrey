#ifndef _SYMBOL
#define _SYMBOL

#include <string>
#include <vector>
#include "utils.h"

using namespace std;

class symbol{
    
    protected:

        string name;

        bool constant;
        
        TYPE type;
        
        int line;
        
        int column;
        
        bool initialized;
        
    public:
        
        bool is_function;
        
        unsigned int offset;
        bool is_local;
        
        symbol();
        
        symbol(string name, bool constant, TYPE type, int line, int column,
               bool initialized);
        
        string getName();

        bool isConst();

        TYPE getType();
        
        int getLine();

        int getColumn();

        bool isInitialized();
};

class symbol_function : public symbol {
    public:

        vector<TYPE> params;

        symbol_function();

        symbol_function(string name, int line, int column,
                        vector< TYPE >& types);
        
        symbol_function(string name, TYPE type, int line, int column,
                        vector< TYPE >& types);
};

#endif
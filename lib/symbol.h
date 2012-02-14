#ifndef _SYMBOL
#define _SYMBOL

#include <string>
#include <vector>

using namespace std;

class symbol{
    public:

        enum TYPE { INT, FLOAT, CHAR, BOOLEAN, NONE, UNDEFINED, INVALID };

    protected:

        string name;

        bool constant;

        TYPE type;

        int line;

        int column;

        bool initialized;
        
    public:
        
        bool is_function;
        
        symbol();
        
        symbol(string name, bool constant, TYPE type, int line, int column, bool initialized);
        
        string getName();

        bool isConst();

        TYPE getType();

        string getTypeName();
        
        static string getTypeName(TYPE t);
        
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
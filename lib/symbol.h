#ifndef __SYMBOL
#define __SYMBOL

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

        symbol();
        
        symbol(string name, bool constant, TYPE type, int line, int column, bool initialized);
        
        string getName();

        bool isConst();

        TYPE getType();

        int getLine();

        int getColumn();

        bool isInitialized();
};

class symbol_function : public symbol {
    public:

        vector<symbol> params;

        symbol_function();

        symbol_function(string name, TYPE type, int line, int column, void* pointer, vector< TYPE > types);//AST_function& f
};

#endif
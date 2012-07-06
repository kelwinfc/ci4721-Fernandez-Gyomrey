#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
#include "utils.h"
#include <set>

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
        bool unused;
        
        unsigned int offset;
        unsigned int width;
        unsigned int alignment;
        unsigned int index;
        
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
        
        // Define en que indices de registro esta disponible el simbolo.
        // El indice del registro es cuestion de implantacion del compilador,
        // no tiene que ver con el nombre del registro real en la maquina
        // destino
        set<int> availability;
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
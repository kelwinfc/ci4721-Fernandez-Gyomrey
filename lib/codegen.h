#ifndef CODEGEN_H
#define CODEGEN_H

#include <set>
#include <vector>
#include "symbol.h"
#include "inst.h"

using namespace std;

class register_descriptor{
    private:
        int reg_num;
        string reg_name;
        set<symbol*> variables_available;
        bool reserved;
    
    public:
        register_descriptor();
        
        register_descriptor(int num, string name);
        
        bool contains(symbol* s);
        
        bool is_free();
        
        int release_cost();
        
        void release();
        
        void load(symbol* s);
};

vector<register_descriptor> register_descriptors;

void getReg(quad* i, int* r0, int* r1, int* r2);

void addReg(int i, string name);

#endif

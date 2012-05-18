#ifndef _BLOCK
#define _BLOCK

#include<list>
#include <cstdio>
#include "inst.h"

using namespace std;

class block{

    protected:
        list<inst*> linst;
        int num_instr;
        
    public:
        block();
        void append_inst(inst* i);
        void dump();
        
        int next_instruction();
        void backpatch(list<int>& l, int instr);
};

#endif
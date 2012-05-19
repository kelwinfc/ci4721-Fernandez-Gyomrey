#ifndef _BLOCK
#define _BLOCK

#include <list>
#include <cstdio>
#include <vector>
#include "inst.h"

using namespace std;

typedef union instructions{
    list<inst*>   *linst;
    vector<inst*> *vinst;
} seq_inst;

class block{
    
    protected:
        
        seq_inst instructions;
        
        int num_instr;
        bool w_vector;
    
    public:
        block(bool with_vector=false);
        void append_inst(inst* i);
        void dump();
        
        int next_instruction();
        void backpatch(list<int>& l, int instr);
};

#endif
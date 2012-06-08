#ifndef _BLOCK
#define _BLOCK

#include <list>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <map>
#include <fstream>
#include "inst.h"

using namespace std;

typedef union instructions{
    list<inst*>   *linst;
    vector<inst*> *vinst;
} seq_inst;

class block{
    
    public:
        int index;
        bool w_vector;
        seq_inst instructions;
        
        block(bool with_vector = false);
        void append_inst(inst *i, bool gen_label = true);
        void append_inst(quad::OP op, opd *arg0, opd *arg1 = 0, opd *arg2 = 0, string comment = "", bool gen_label = true);
        inst* last_instruction();
        
        void dump(ostream &strm, bool with_comments = false);
        
        block* mandatory_exit;
        vector<block*> sucessors;
        
        int next_instruction();
        void backpatch(list<int>& l, int instr);
        
        unsigned int max_label();
        void gen_graph();
};

void dump_in_file(string filename);

#endif
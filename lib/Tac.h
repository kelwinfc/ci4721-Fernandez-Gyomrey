#ifndef TAC_H
#define TAC_H

#include<list>
#include<iostream>
#include "block.h"
#include "inst.h"
#include "optimizer.h"
#include "Spim.h"

using namespace std;

extern unsigned int next_label;

extern vector<Block*> list_of_blocks;

class Spim;

class Tac{

    private:
        void* iterate_next(inst** i, void* it = 0);

    public:

        Block* block;

        Tac();

        void append_quad(quad::OP op, opd *arg0, opd *arg1, opd *arg2, string comment);

        int next_instruction();

        void backpatch(list<int>& l, int instr);

        void gen_graph();

        void optimize();

        void dump(ostream &strm, bool with_comments);

        Spim* to_spim();
};

#endif
#ifndef _Tac
#define _Tac

#include<list>
#include<iostream>
#include "block.h"
#include "inst.h"
#include "optimizer.h"

extern unsigned int next_label;

class Tac{

    public:

        Block* block;

        Tac();

        void append_quad(quad::OP op, opd *arg0, opd *arg1, opd *arg2, string comment);

        int next_instruction();

        void backpatch(list<int>& l, int instr);

        void gen_graph();

        void optimize();

        void dump(ostream &strm, bool with_comments);
};

#endif
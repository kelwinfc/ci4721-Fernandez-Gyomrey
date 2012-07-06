#ifndef SPIM_H
#define SPIM_H

#include "inst.h"
#include "Tac.h"
#include<vector>
#include<iostream>

using namespace std;

class Tac;

class Spim{
    public:
        Tac* tac;

        Block* block;

        vector<inst*> instructions;

        Spim(Tac* tac);

        void append_inst(quad::OP op, opd *arg0, opd *arg1, opd *arg2, string comment);

        void dump(ostream& strm, bool with_comments);
};

#endif
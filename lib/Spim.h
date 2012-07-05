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

        vector<inst*> instructions;

        Spim(Tac* tac);

        void append_inst(inst* i);

        void dump(ostream& strm, bool with_comments);
};

#endif
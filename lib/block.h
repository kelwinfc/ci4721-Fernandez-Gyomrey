#ifndef _BLOCK
#define _BLOCK

#include<list>
#include <cstdio>
#include "inst.h"

using namespace std;

class block{

    protected:
        list<inst*> linst;

    public:
        block();
        void append_inst(inst* i);
        void dump();
};

#endif
#ifndef _BLOCK
#define _BLOCK

#include<list>
#include "inst.h"

using namespace std;

class block{

    protected:
        list<inst*> linst;

    public:
        block();
        list<inst*> get_inst();
};

#endif
#include "inst.h"

int newtemp(){
    static int t = 0;
    return t ++;
}

quad::quad(OP op, int arg0, int arg1, int arg2){
    this->op = op;
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
}
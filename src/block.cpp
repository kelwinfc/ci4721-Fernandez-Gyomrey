#include "block.h"
#include <iostream>

using namespace std;

block::block(bool with_vector){
    num_instr = 0;
    w_vector = with_vector;
    if ( with_vector ){
        instructions.vinst = new vector<inst*>();
    } else {
        instructions.linst = new list<inst*>();
    }
}

void block::append_inst(inst* i) {
    
    if ( w_vector ){
        instructions.vinst->push_back(i);
    } else {
        instructions.linst->push_back(i);
    }
    
    num_instr++;
}

void block::dump() {
    int index = 0;
    if ( w_vector ){
        vector<inst*>::iterator it = instructions.vinst->begin();
        for (; it != instructions.vinst->end(); ++ it) {
            cout << "L" << index++ << ": ";
            printf("%s\n", ((quad*)(*it))->to_string().c_str());
        }
    } else {
        list<inst*>::iterator it = instructions.linst->begin();
        for (; it != instructions.linst->end(); ++ it) {
            cout << "L" << index++ << ": ";
            printf("%s\n", ((quad*)(*it))->to_string().c_str());
        }
    }
}

int block::next_instruction(){
    return num_instr;
}

//TODO
void block::backpatch(list<int>& l, int instr){
    list<int>::iterator it;
    for (it = l.begin(); it != l.end(); ++it){
        quad* in = (quad*)(*(instructions.vinst))[*it];
        in->arg2 = new opd(instr,true);
    }
}
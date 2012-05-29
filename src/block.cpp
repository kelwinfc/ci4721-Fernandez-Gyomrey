#include "block.h"
#include <iostream>

unsigned int next_label = 0;
vector<block*> list_of_blocks;

using namespace std;

block::block(bool with_vector){
    w_vector = with_vector;
    if ( with_vector ){
        instructions.vinst = new vector<inst*>();
    } else {
        instructions.linst = new list<inst*>();
    }
}

void block::append_inst(inst* i, bool gen_label) {
    
    int ninst = next_instruction();
    if ( w_vector ){
        instructions.vinst->push_back(i);
        
        if ( gen_label ){
            instructions.vinst->back()->label = ninst;
        }
    } else {
        instructions.linst->push_back(i);
        
        if ( gen_label ){
            instructions.linst->back()->label = ninst;
        }
    }
    
    if ( gen_label ){
        next_label++;
    }
}

inst* block::last_instruction(){
    if ( w_vector ){
        return instructions.vinst->back();
    } else {
        return instructions.linst->back();
    }
}

void block::dump() {
    if ( w_vector ){
        vector<inst*>::iterator it = instructions.vinst->begin();
        for (; it != instructions.vinst->end(); ++ it) {
            printf("%s\n", ((quad*)(*it))->to_string().c_str());
        }
    } else {
        list<inst*>::iterator it = instructions.linst->begin();
        for (; it != instructions.linst->end(); ++ it) {
            printf("%s\n", ((quad*)(*it))->to_string().c_str());
        }
    }
}

int block::next_instruction(){
    return next_label;
}

//TODO
void block::backpatch(list<int>& l, int instr){
    list<int>::iterator it;
    for (it = l.begin(); it != l.end(); ++it){
        quad* in = (quad*)(*(instructions.vinst))[*it];
        in->arg2 = new opd(instr,true);
    }
}
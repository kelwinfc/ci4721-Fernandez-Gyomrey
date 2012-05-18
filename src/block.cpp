#include "block.h"

block::block(){
    num_instr = 0;
}

void block::append_inst(inst* i) {
    linst.push_back(i);
    num_instr++;
}

void block::dump() {

    for (list<inst*>::iterator it = linst.begin(); it != linst.end(); ++ it) {
        printf("%s\n", ((quad*)(*it))->to_string().c_str());
    }
}

int block::next_instruction(){
    return num_instr;
}

//TODO
void block::backpatch(list<int>& l, int instr){
    
}
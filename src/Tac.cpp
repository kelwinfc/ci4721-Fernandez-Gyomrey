#include "Tac.h"

extern vector<Block*> list_of_blocks;

Tac::Tac(){
    block = new Block(true);
}


void Tac::append_quad(quad::OP op, opd *arg0, opd *arg1, opd *arg2, string comment) {
    block->append_inst(new quad(op, arg0, arg1, arg2, comment), true);
}

int Tac::next_instruction(){
    return next_label;
}

void Tac::backpatch(list<int>& l, int instr){
    list<int>::iterator it;
    for (it = l.begin(); it != l.end(); ++it){
        quad* in = (quad*)(*(block->instructions.vinst))[*it];
        in->arg2 = new opd(instr,true);
    }
}

void Tac::gen_graph(){
    block->gen_graph();
}

void Tac::optimize(){
    delete_unreachable_code();
}

void Tac::dump(ostream &strm, bool with_comments) {
    block->dump(strm, with_comments);
}

Spim* Tac::to_spim()
{
    return new Spim();
}
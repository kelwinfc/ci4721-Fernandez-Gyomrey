#include "Tac.h"

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
        in->arg2 = new opd(instr, O_LABEL);
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
    Spim* spim = new Spim(this);
    
    vector<Block*>::iterator itb = list_of_blocks.begin();
    bool in = false, out = false;
    for( ; itb != list_of_blocks.end(); ++itb){
        list<inst*>::iterator iti = (*itb)->instructions.linst->begin();
        for( ; iti != (*itb)->instructions.linst->end(); ++iti){
            const inst i = **iti;
            in = !out && (in || (i.op == inst::PROLOGUE && !i.arg2->data.sym->getName().compare("main")));
            if (in) {
                switch (i.op) {
                case inst::WRITE:
                    //spim->append_inst(Spim_Inst::LI);
                    //spim->append_inst(Spim_Inst::LI);
                default:
                    break;
                }
                //spim->append_inst(*iti);
            }
            out = out || (i.op == inst::EPILOGUE && !i.arg2->data.sym->getName().compare("main"));
            if (out && in) {
                // epílogo main
                //spim->append_inst(Spim_Inst::LI, new opd(10));
                //spim->append_inst(Spim_Inst::LI);
            }
        }
    }
    return spim;
}
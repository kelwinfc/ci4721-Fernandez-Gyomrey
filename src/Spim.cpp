#include "Spim.h"

unsigned int next_label_spim = 0;

Spim::Spim(Tac* tac){
    this->tac = tac;
}

void Spim::append_inst(Spim_Inst::OP op, opd *arg0, opd *arg1, opd *arg2, string comment){
    Spim_Inst* i = new Spim_Inst(op, arg0, arg1, arg2, comment);
    i->label = ++ next_label_spim;
    instructions.push_back(i);
}

void Spim::dump(ostream& strm, bool with_comments){
    vector<inst*>::iterator it = instructions.begin();
    for (; it != instructions.end(); ++ it) {
        strm << ((inst*)(*it))->to_string(with_comments) << endl;
    }
}
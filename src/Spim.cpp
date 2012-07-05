#include "Spim.h"

Spim::Spim(Tac* tac){
    this->tac = tac;
}

void Spim::append_inst(inst* i){
    instructions.push_back(i);
}

void Spim::dump(ostream& strm, bool with_comments){
    vector<inst*>::iterator it = instructions.begin();
    for (; it != instructions.end(); ++ it) {
        strm << ((inst*)(*it))->to_string(with_comments) << endl;
    }
}
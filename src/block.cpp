#include "block.h"

block::block(){

}

void block::append_inst(inst* i) {
    linst.push_back(i);
}

void block::dump() {

    for (list<inst*>::iterator it = linst.begin(); it != linst.end(); ++ it) {
        printf("%s\n", ((quad*)(*it))->to_string().c_str());
    }
}
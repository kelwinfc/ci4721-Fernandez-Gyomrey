#include "block.h"

block::block(){

}

void block::append_inst(inst* i) {
    linst.push_back(i);
}

void block::dump() {

}
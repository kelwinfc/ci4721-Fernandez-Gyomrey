#include "optimizer.h"

using namespace std;

void next_instruction_jumps(block& code){
    if ( code.w_vector ){
        unsigned int nsize = code.instructions.vinst->size();
        
        // Guardar las instrucciones que no sean saltos a la siguiente
        // instruccion
        vector<inst*> new_insts;
        for (unsigned int i=0; i+1<nsize; i++){
            quad* current_inst = (quad*)(*(code.instructions.vinst))[i];
            quad* next_inst    = (quad*)(*(code.instructions.vinst))[i+1];
            
            if (    current_inst->op == quad::GOTO 
                 && current_inst->get_goal_label() == next_inst->label
               )
            {
                delete current_inst;
                (*(code.instructions.vinst))[i] = 0;
            } else {
                new_insts.push_back( current_inst );
            }
        }
        if ( nsize ){
            new_insts.push_back( code.instructions.vinst->back() );
        }
        unsigned int goal_size = new_insts.size();
        
        code.instructions.vinst->clear();
        for (unsigned int i=0; i<goal_size; i++){
            code.instructions.vinst->push_back( new_insts[i] );
        }
    }
}

void fall_jumps(block& code){
    
}

void useless_jumps(block& code){
    next_instruction_jumps(code);
    fall_jumps(code);
}

#include "optimizer.h"
#include <iostream>
#include <stack>

using namespace std;

extern vector<Block*> list_of_blocks;
extern unsigned int first_instruction;
extern map<unsigned int, Block*> label_to_block;

void next_instruction_jumps(Block& code){
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

void fall_jumps(Block& code){
    
}

void useless_jumps(Block& code){
    next_instruction_jumps(code);
    fall_jumps(code);
}

void delete_unreachable_code(){
    
    int* reachable;
    reachable = (int*)malloc(list_of_blocks.size()*sizeof(int));
    memset(reachable, 0, list_of_blocks.size()*sizeof(int));
    
    stack<Block*> s;
    {
        Block* first_block = 0;
        
        vector<Block*>::iterator it = list_of_blocks.begin();
        for( ; it != list_of_blocks.end(); ++it){
            if ( (*((*it)->instructions.linst->begin()))->label == first_instruction ){
                first_block = *it;
                break;
            }
        }
        
        if ( first_block == 0 ){
            return;
        }
        
        s.push(first_block);
    }
    
    while ( !s.empty() ){
        Block* next_block = s.top();
        s.pop();
        
        if ( reachable[next_block->index] )
            continue;
        
        reachable[next_block->index] = 1;
        
        if ( next_block->mandatory_exit != 0 ){
            s.push( next_block->mandatory_exit );
        }
        
        vector<Block*>::iterator it = next_block->sucessors.begin();
        for (; it != next_block->sucessors.end(); ++it ){
            s.push(*it);
        }
    }
    
    vector<Block*> reachable_blocks;
    for (uint i = 0; i<list_of_blocks.size(); i++){
        if ( reachable[ list_of_blocks[i]->index ] ){
            reachable_blocks.push_back(list_of_blocks[i]);
        } else {
            //delete list_of_blocks[i];
        }
    }
    while (!list_of_blocks.empty()){
        list_of_blocks.pop_back();
    }
    for (uint i=0; i<reachable_blocks.size(); i++){
        list_of_blocks.push_back(reachable_blocks[i]);
    }
}
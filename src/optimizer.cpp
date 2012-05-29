#include "optimizer.h"
#include <iostream>

using namespace std;

extern vector<block*> list_of_blocks;

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

unsigned int max_label(block& b){
    
    unsigned int maxl = 0;
    
    if ( b.w_vector ){
        
        unsigned int nsize = b.instructions.vinst->size();
        for (unsigned int i=0; i<nsize; i++){
            inst* current_inst = (*(b.instructions.vinst))[i];
            maxl = max(maxl, current_inst->label);
        }
        
    } else {
        
        list<inst*>::iterator it = b.instructions.linst->begin();
        for( ; it != b.instructions.linst->end(); ++it){
            inst* current_inst = *it;
            maxl = max(maxl, current_inst->label);
        }
    }
    
    return maxl;
}

#define SET_LEADER(a,i) (a[(i)/32] = a[(i)/32] | ( 1 << ((i) % 32 )))
#define IS_LEADER(a,i)  (a[(i)/32] & (1 << ((i) % 32)))

void gen_graph(block& src, block& dst){
    
    // Marcar los lideres de grupo
    int* leaders;
    leaders = (int*)malloc( (max_label(src)/32 + 1)*sizeof(int));
    memset(leaders, 0, sizeof leaders);
    
    vector<inst*>::iterator it = src.instructions.vinst->begin();
    
    bool was_jump = true; // indica si la instruccion anterior fue un salto
    unsigned int index;
    for (index = 0; index != src.instructions.vinst->size(); index++){
        
        inst* current_inst = (*(src.instructions.vinst))[index];
        
        if ( was_jump ){
            SET_LEADER(leaders, current_inst->label);
            was_jump = false;
        }
        
        if ( current_inst->is_jump() && ((quad*)current_inst)->arg2 ){
            int destino = ((quad*)current_inst)->arg2->pint;
            SET_LEADER(leaders, destino);
            
            was_jump = true;
        }
    }
    
    cout << "Leaders:";
    for (uint index = 0; index != src.instructions.vinst->size(); index++){
        inst* current_inst = (*(src.instructions.vinst))[index];
        
        if ( IS_LEADER(leaders, current_inst->label) ){
            cout << " " << current_inst->label;
        }
    }
    cout << endl;
    
    map<unsigned int, block*> label_to_block;
    
    // Creacion de los bloques basicos
    index = 0;
    while( index < src.instructions.vinst->size() ){
        
        // Agregar el siguiente bloque basico a la lista global de bloques
        block* next_block = new block(false);
        next_block->index = list_of_blocks.size();
        
        list_of_blocks.push_back(next_block);
        
        //cout << "B" << list_of_blocks.size() << ":";
        
        // Añadir todas las instrucciones del bloque basico
        inst* current_inst = (*(src.instructions.vinst))[index];
        label_to_block[ current_inst->label ] = list_of_blocks.back();
        
        do {
            list_of_blocks.back()->append_inst(current_inst, false);
            
            //cout << " " << current_inst->label;
            
            index++;
            current_inst = (*(src.instructions.vinst))[index];
            
        } while( index < src.instructions.vinst->size() 
                    && !IS_LEADER(leaders, current_inst->label)
               );
        //cout << endl;
    }
    
    // En este punto ya se puede eliminar el arreglo de lideres. Los lideres son
    // la primera instruccion de cada bloque basico y la ultima instruccion de
    // cada uno marca el destino alternativo
    delete leaders;
    
    // Conectar los bloques
    for (uint block_index = 0 ; block_index != list_of_blocks.size();
         block_index++)
    {
        block* current_block = list_of_blocks[block_index];
        
        cout << "B" << block_index << endl;
        
        if ( block_index + 1 < list_of_blocks.size() ){
            current_block->mandatory_exit = list_of_blocks[block_index+1];
            
            cout << "  --> B" << block_index+1 << endl;
        } else {
            current_block->mandatory_exit = 0;
        }
        
        inst* last_inst = current_block->last_instruction();
        
        if ( last_inst->is_jump() && ((quad*)last_inst)->arg2 != 0 ){
            
            current_block->sucessors.push_back( label_to_block[((quad*)last_inst)->arg2->pint] );
            cout << "  --> B"
                 << current_block->sucessors.back()->index << endl;
        }
    }
}

#include "block.h"
#include <iostream>

unsigned int next_label = 0;

extern map<string,int> func_to_prologue;
extern map<string,int> func_to_epilogue;

unsigned int first_instruction = 0;

using namespace std;

Block::Block(bool with_vector){
    w_vector = with_vector;
    if ( with_vector ){
        instructions.vinst = new vector<inst*>();
    } else {
        instructions.linst = new list<inst*>();
    }
}

void Block::append_inst(inst *i, bool gen_label) {

    int ninst = next_instruction();
    if ( w_vector ){
        instructions.vinst->push_back(i);
        
        if ( gen_label ){
            instructions.vinst->back()->label = ninst;
        }
    } else {
        instructions.linst->push_back(i);
        
        if ( gen_label ){
            instructions.linst->back()->label = ninst;
        }
    }
    
    if ( gen_label ){
        next_label++;
    }
}

inst* Block::last_instruction(){
    if ( w_vector ){
        return instructions.vinst->back();
    } else {
        return instructions.linst->back();
    }
}

void Block::dump(ostream &strm, bool with_comments) {
    if ( w_vector ){
        vector<inst*>::iterator it = instructions.vinst->begin();
        for (; it != instructions.vinst->end(); ++ it) {
            strm << ((inst*)(*it))->to_string(with_comments) << endl;
        }
    } else {
        list<inst*>::iterator it = instructions.linst->begin();
        for (; it != instructions.linst->end(); ++ it) {
            strm << ((inst*)(*it))->to_string(with_comments) << endl;
        }
    }
}

int Block::next_instruction(){
    return next_label;
}

unsigned int Block::max_label(){
    
    unsigned int maxl = 0;
    
    if ( w_vector ){
        
        unsigned int nsize = instructions.vinst->size();
        for (unsigned int i=0; i<nsize; i++){
            inst* current_inst = (*(instructions.vinst))[i];
            maxl = max(maxl, current_inst->label);
        }
        
    } else {
        
        list<inst*>::iterator it = instructions.linst->begin();
        for( ; it != instructions.linst->end(); ++it){
            inst* current_inst = *it;
            maxl = max(maxl, current_inst->label);
        }
    }
    
    return maxl;
}


#define SET_LEADER(a,i) (a[(i)/32] = a[(i)/32] | ( 1 << ((i) % 32 )))
#define IS_LEADER(a,i)  (a[(i)/32] & (1 << ((i) % 32)))

void Block::gen_graph(){
    
    // Marcar los lideres de grupo
    int* leaders;
    leaders = (int*)malloc( (max_label()/32 + 1)*sizeof(int));
    memset(leaders, 0, (max_label()/32 + 1)*sizeof(int) );
    
    vector<inst*>::iterator it = instructions.vinst->begin();
    
    bool was_jump = true; // indica si la instruccion anterior fue un salto
    unsigned int index;
    for (index = 0; index != instructions.vinst->size(); index++){
        
        inst* current_inst = (*(instructions.vinst))[index];
        
        if ( was_jump ){
            SET_LEADER(leaders,current_inst->label);
            was_jump = false;
        }
        
        if ( current_inst->is_jump() ){
            
            if ( ((inst*)current_inst)->op == inst::CALL )
            {
                int destino = 
                  func_to_prologue[((inst*)current_inst)->arg1->data.sym->getName()];
                SET_LEADER(leaders,destino);
                was_jump = true;
            } else if ( ((inst*)current_inst)->op == inst::EPILOGUE )
            {
                int destino = 
                  func_to_epilogue[((inst*)current_inst)->arg2->data.sym->getName()];
                  SET_LEADER(leaders,destino);
                was_jump = true;
            } else if (((inst*)current_inst)->arg2 != 0) {
                unsigned int destino = ((inst*)current_inst)->arg2->data.pint;
                SET_LEADER(leaders,destino);
                was_jump = true;
            }
        }
    }

    /* este es el único lugar en el que se usa cout fuera de main.
       si es importante conocer los líderes, entonces deberían ser
       conservados hasta el final para poder ser impresos. de otra
       forma no se tiene control sobre la impresión de este segmento
    cout << "lideres:";
    for (uint i=0; i<instructions.vinst->size(); i++){
        if ( IS_LEADER(leaders, i) ){
            cout << " " << i;
        }
    }
    cout << endl;
    */
    
    map<unsigned int, Block*> label_to_block;
    
    // Creacion de los bloques basicos
    index = 0;
    
    while( index < instructions.vinst->size() ){
        
        // Agregar el siguiente bloque basico a la lista global de bloques
        Block* next_block = new Block(false);
        next_block->index = list_of_blocks.size();
        
        list_of_blocks.push_back(next_block);
        
        // Añadir todas las instrucciones del bloque basico
        inst* current_inst = (*(instructions.vinst))[index];
        label_to_block[ current_inst->label ] = list_of_blocks.back();
        
        do {
            list_of_blocks.back()->append_inst(current_inst, false);
            
            index++;
            current_inst = (*(instructions.vinst))[index];
            
        } while( index < instructions.vinst->size() 
                    && !IS_LEADER(leaders,current_inst->label)
               );
    }
    
    // En este punto ya se puede eliminar el arreglo de lideres. Los lideres son
    // la primera instruccion de cada bloque basico.
    free(leaders);
    
    // Conectar los bloques
    for (uint block_index = 0 ; block_index != list_of_blocks.size();
         block_index++)
    {        
        Block* current_block = list_of_blocks[block_index];
        
        /* Definicion de salida obligatoria
         * Se coloca que la salida obligatoria de una funcion call es la
         * siguiente para mantener informacion de la secuencialidad.
         * En caso de no desear este comportamiento colocar en inst el operador
         * call como obligatorio
         */
        current_block->mandatory_exit = 0;
        if ( block_index + 1 < list_of_blocks.size()
             && (    !list_of_blocks[block_index]->last_instruction()->is_jump()
                  || !list_of_blocks[block_index]->last_instruction()
                          ->mandatory_jump() )
           )
        {
                current_block->mandatory_exit = list_of_blocks[block_index+1];
        }
        
        inst* last_inst = current_block->last_instruction();
        
        if ( last_inst->is_jump() && ((inst*)last_inst)->op == inst::GOTO ){
            current_block->mandatory_exit = 
                label_to_block[ ((inst*)last_inst)->arg2->data.pint ];
        }
        
        if ( ((inst*)last_inst)->op == inst::RETURN ){
            current_block->mandatory_exit = 
                label_to_block[((inst*)last_inst)->arg2->data.pint];
        }
        
        // Marcar las salidas opcionales
        if ( last_inst->is_jump() && !last_inst->mandatory_jump() )
        {
            if ( ((inst*)last_inst)->op == inst::CALL ){
                string func = ((inst*)last_inst)->arg1->data.sym->getName();
                
                // Agregar salto desde el llamador al prologo del llamado
                current_block->sucessors.push_back(
                    label_to_block[ func_to_prologue[func]]);
                
                // Agregar salto desde el epilogo del llamado a la salida
                // obligatoria de este bloque (siguiente instruccion despues
                // de la llamada)
                label_to_block[func_to_epilogue[func]]->sucessors.push_back(
                    current_block->mandatory_exit);
            } else if ( ((inst*)last_inst)->arg2 != 0 ){
                current_block->sucessors.push_back(
                            label_to_block[((inst*)last_inst)->arg2->data.pint] );
            }
        }
    }
    
}

void dump_blocks_definition(string filename){
    ofstream fout;
    
    fout.open((filename+".blocks").c_str());
    
    for (uint index = 0 ; index != list_of_blocks.size(); index++)
    {
        // Bloque basico
        fout << " B" << list_of_blocks[index]->index << ":\n";
        
        fout << "+--------------------------\n";
        
        // Lista de instrucciones
        list<inst*>::iterator it = 
            list_of_blocks[index]->instructions.linst->begin();
        for ( ; it != list_of_blocks[index]->instructions.linst->end(); ++it ){
            inst* current_inst = *it;
            fout << "| " << current_inst->to_string(false) << endl;
        }
        fout << "|\n";
        
        // Salida obligatoria
        if ( list_of_blocks[index]->mandatory_exit ){
            fout << "| --> B" << list_of_blocks[index]->mandatory_exit->index;
        } else {
            fout << "| --> ";
        }
        fout << endl;
        
        // Salidas opcionales
        if ( list_of_blocks[index]->sucessors.size() != 0 ){
            vector<Block*>::iterator it =
            list_of_blocks[index]->sucessors.begin();
            fout << "| -->";
            for ( ; it != list_of_blocks[index]->sucessors.end(); ++it ){
                fout << " B" << (*it)->index;
            }
            fout << endl;
        }
        
        fout << "+--------------------------\n\n";
    }
    
    fout.close();
}

void dump_in_file(string filename){
    
    dump_blocks_definition("bla");
    
    ofstream fout;
    
    fout.open((filename+".gv").c_str());
    fout << "##Command to produce the output: "
         << "\"neato -Tpng bla.gv > bla.png\"\n"
         << "digraph GrafoDeBloquesBasicos {\n"
         << "    node [shape=circle,width=0.1];";
    for (uint block_index = 0 ; block_index != list_of_blocks.size();
         block_index++)
    {
        fout << " B" << list_of_blocks[block_index]->index << ";";
    }
    fout << endl;
    
    for (uint block_index = 0 ; block_index != list_of_blocks.size();
         block_index++)
    {
        if ( list_of_blocks[block_index]->mandatory_exit ){
            fout << "B" << list_of_blocks[block_index]->index
                 << "->B"
                 << list_of_blocks[block_index]->mandatory_exit->index
                 << ";" << endl;
        }
        
        vector<Block*>::iterator it =
            list_of_blocks[block_index]->sucessors.begin();
        for ( ; it != list_of_blocks[block_index]->sucessors.end(); ++it ){
            fout << "B" << list_of_blocks[block_index]->index
                 << "->B"
                 << (*it)->index
                 << ";" << endl;
        }
    }
    
    fout << endl
         << "overlap=false\n"
         << "label=\"Grafo de Bloques Basicos\"\n"
         << "fontsize=8;\n"
         << "}\n";
    fout.close();
    
    string command = "rm -rf " + filename + ".png; neato -Tpng "
                        + filename + ".gv > "+ filename + ".png";
    int salida = system(command.c_str());
    
    if ( salida != 0 ){
        cout << "Ocurrió un error al ejecutar el comando neato para generar el"
             << "grafo.\n";
    }
}
#include "codegen.h"

register_descriptor::register_descriptor(){
    reg_num = 0;
    reg_name = "";
}

register_descriptor::register_descriptor(int num, string name){
    reg_num  = num;
    reg_name = name;
}

bool register_descriptor::contains(symbol* s){
    return variables_available.find(s) != variables_available.end();
}

bool register_descriptor::is_free(){
    return variables_available.size() == 0;
}

int register_descriptor::release_cost(){
    
    int cost = 0;
    set<symbol*>::iterator it = variables_available.begin();
    
    for ( ; it != variables_available.end(); ++it){
        symbol* s = *it;
        if ( s->availability.size() == 1 )
            cost++;
    }
    
    return cost;
}

//TODO
void register_descriptor::release(){
    
}

void register_descriptor::load(symbol* s){
    variables_available.insert(s);
    s->availability.insert(reg_num);
    
    //TODO: generar linea de codigo que haga el load
}

void addReg(int i, string name){
    register_descriptor rd(i, name);
    register_descriptors.push_back(rd);
}

//TODO
int get_reg(symbol* s){
    
    int index = 0;
    int empty = -1;
    
    vector<register_descriptor>::iterator it = register_descriptors.begin();
    
    if ( s ){
        // Si ya esta en un registro, aprovecharlo
        for( ; it != register_descriptors.end(); ++it){
            if ( it->contains(s) )
                return index;
            else if ( it->is_free() ){
                empty = index;
            }
            index++;
        }
        
        // El simbolo no se encuentra ya en un registro
        
        // Hay algun registro vacio, usarlo
        if ( empty != -1 ){
            return empty;
        }
    }
    
    int cheapest_register = 0;
    int cost = register_descriptors[0].release_cost();
    
    it = register_descriptors.begin()+1;
    index = 0;
    for( ; it != register_descriptors.end(); ++it){
        int next_cost = it->release_cost();
        
        if ( next_cost < cost ){
            cheapest_register = index;
            cost = next_cost;
        }
        index++;
    }
    
    //TODO liberar lo que sea necesario liberar (hacer los ST respectivos)
    //TODO cargar el simbolo en un registro
    return cheapest_register;
}

void getReg(quad* i, int* r0, int* r1, int* r2){
    //TODO 
}

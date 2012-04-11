#include "label_generator.h"

label_generator::label_generator(){
    last = 0;
}

ULLI label_generator::next_label(){
    if ( unused_labels.size() == 0 ){
        if ( last > last + 1 ){
            fprintf(stderr, "Excedido el numero maximo de etiquetas\n");
        }
        return last++;
    } else {
        
        ULLI next = unused_labels.back();
        unused_labels.pop_back();
        
        return next;
    }
}

void label_generator::mark_unused(ULLI label){
    unused_labels.push_back(label);
}

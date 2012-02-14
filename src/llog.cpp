#include <stdio.h>
#include <iostream>

#include "llog.h"

llog::llog(){
    registered_error = false;
}

void llog::error(int l, int c, const char* e){
    //TODO hay que copiar e si queremos que no se pierda al eliminar e del contexto padre
    fprintf(stderr, "Error %d:%d: %s\n", l, c, e);
    registered_error = true;
}

void llog::critical(const char* m){
    throw m;
}

void llog::failure(const char* c){
    fprintf(stderr, "Epic fail !(%s)\n", c);
}

void llog::success(){
    cout << "Like a boss!\n";
    cout << "-------------------------------------------------------\n";
}

bool llog::exists_registered_error(){
    return registered_error;
}
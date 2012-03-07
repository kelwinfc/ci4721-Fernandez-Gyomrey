#include <stdio.h>
#include <iostream>

#include "llog.h"

llog::llog(){
    registered_error = false;
}

void llog::warning(int l, int c, const char* e){
    if ( e ){
        fprintf(stderr, "Warning %d:%d: %s\n", l, c, e);
    }
}

void llog::error(int l, int c, const char* e){
    //TODO hay que copiar e si queremos que no se pierda al eliminar e del contexto padre
    if ( e ){
        fprintf(stderr, "Error %d:%d: %s\n", l, c, e);
    }
    registered_error = true;
}

void llog::critical(const char* m){
    fprintf(stderr, "%s", m);
    abort();
}

void llog::failure(const char* c){
    fprintf(stderr, "-------------------------------------------------------\n"
           );
    fprintf(stderr, "\n\n\nEpic fail! (%s)\n", c);
}

void llog::success(){
    fprintf(stderr, "-------------------------------------------------------\n"
           );
    cout << "\n\n\nLike a boss!\n";
}

bool llog::exists_registered_error(){
    return registered_error;
}
#include <stdio.h>
#include <iostream>
#include <cstring>

#include "llog.h"

llog::llog(){
    registered_error = false;
}

void llog::warning(int l, int c, const char* e){
    if ( e ){
        char fin[llog::ERR_LEN * 2];
        sprintf(fin, "Warning %d:%d: %s\n", l, c, e);
        stringstream ss;
        string err;
        ss << fin;
        getline(ss, err);
        messages[l][c].push_back(err);
    }
}

void llog::error(int l, int c, const char* e){
    if ( e && strcmp( (char*)"parse error", e) ){
        char fin[llog::ERR_LEN * 2];
        sprintf(fin, "Error   %d:%d: %s\n", l, c, e);
        stringstream ss;
        string err;
        ss << fin;
        getline(ss, err);
        messages[l][c].push_back(err);
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

void llog::dump()
{
    map<int, map<int, vector<string> > >::iterator itl;
    for (itl = messages.begin(); itl != messages.end(); ++ itl) {
        map<int, vector<string> >::iterator itc;
        for (itc = (*itl).second.begin(); itc != (*itl).second.end(); ++ itc) {
            vector<string>::iterator itm;
            for (itm = (*itc).second.begin(); itm != (*itc).second.end(); ++ itm) {
                fprintf(stderr, "%s\n", (*itm).c_str());
            }
        }
    }
}
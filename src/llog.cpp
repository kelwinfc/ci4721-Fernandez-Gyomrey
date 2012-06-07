#include <stdio.h>
#include <iostream>
#include <cstring>

#include "llog.h"

llog::llog(){
    buf.str("");
    buf.clear();
    registered_error = false;
}

void llog::warning(int l, int c){
    string b = buf.str();
    buf.str("");
    buf << "Warning " << l << ":" << c << ": "<< b;
    messages[l][c].push_back(buf.str());
    buf.str("");
}

void llog::error(int l, int c){
    string b = buf.str();
    buf.str("");
    if (b.compare("parse error")) {
        buf << "Error   " << l << ":" << c << ": "<< b;
        messages[l][c].push_back(buf.str());
        buf.str("");
    }
    registered_error = true;
}

bool llog::exists_registered_error(){
    return registered_error;
}

void llog::dump(ostream &errd)
{
    map<int, map<int, vector<string> > >::iterator itl;
    for (itl = messages.begin(); itl != messages.end(); ++ itl) {
        map<int, vector<string> >::iterator itc;
        for (itc = (*itl).second.begin(); itc != (*itl).second.end(); ++ itc) {
            vector<string>::iterator itm;
            for (itm = (*itc).second.begin(); itm != (*itc).second.end(); ++ itm) {
                errd << *itm << endl;
            }
        }
    }
}
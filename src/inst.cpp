#include "inst.h"

int newtemp(){
    static int t = 0;
    return ++ t;
}

opd::opd(symbol *s){
    sym = s;
    type = O_SYM;
}

opd::opd(){
    temp = newtemp();
    type = O_TEMP;
}

opd::opd(int s){
    pint = s;
    type = O_INT;
}

opd::opd(char c){
    pchar = c;
    type = O_CHAR;
}

opd::opd(bool s){
    pbool = s;
    type = O_BOOL;
}

opd::opd(float s){
    pfloat = s;
    type = O_FLOAT;
}

string opd::to_string() {
    std::stringstream out; 
    switch (type) {
        case O_SYM:   return sym->getName() + ":S";
        case O_TEMP:  out << temp; return out.str() + ":T";
        case O_INT:   out << pint; return out.str() + ":I";
        case O_CHAR:  return string(1, pchar) + ":C";
        case O_BOOL:  return (pbool ? "true:B" : "false:B");
        case O_FLOAT: out << pfloat; return out.str() + ":D";
    }
    return ":UKNOWN_TYPE";
}

string inst::to_string(){
    return "";
}


quad::quad(OP op, opd *arg0, opd *arg1, opd *arg2){
    this->op = op;
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

string quad::to_string(){
    string r = "";
    opd *opds[] = {arg0, arg1, arg2};
    for (int i = 0; i < 3; ++ i) {
        if (opds[i]) {
            r += (0 == i ? "" : ", ") + opds[i]->to_string();
        }
    }
    switch (op) {
        case CP:     return "CP " + r;
        case ADD:    return "ADD " + r;
        case CALL:   return "CALL " + r;
        case PARAM:  return "PARAM " + r;
        case ACCESS: return "ACCESS " + r;
        case REF:    return "REF " + r;
        case DEREF:  return "DEREF " + r;
        case LD:     return "LD " + r;
    }
    return r;
}
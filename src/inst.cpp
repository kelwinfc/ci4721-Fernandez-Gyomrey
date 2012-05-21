#include "inst.h"
#include <iostream>
#include <cassert>

using namespace std;

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

opd::opd(int s, int label){
    
    pint = s;
    
    if ( label ){
        type = O_LABEL;
    } else {
        type = O_INT;
    }
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
        case O_LABEL: out << pint; return "L" + out.str();
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
    
    // Modo alternativo de impresion: si es preferible el otro basta con remover
    // este segmento, me parece mas claro para Novich imprimir x := a + b que
    // ADD x a b, o x := a que CP x a.
    switch (op ){
        case CP:
            return opds[0]->to_string() + " := " + opds[1]->to_string();
        case GOTO:
            return "goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFL:
            return "if " + opds[0]->to_string() + " < " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFLEQ:
            return "if " + opds[0]->to_string() + " <= " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IF:
            return "if " + opds[0]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case ADD:
            return opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " + " + opds[2]->to_string();
        case SUB:
            return opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " - " + opds[2]->to_string();
        case MUL:
            return opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " * " + opds[2]->to_string();
        case DIV:
            return opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " / " + opds[2]->to_string();
        case MOD:
            return opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " % " + opds[2]->to_string();
        case UMINUS:
            return opds[0]->to_string() + " := -" + opds[1]->to_string();
        default:
            break;
    }
    
    for (int i = 0; i < 3; ++ i) {
        if (opds[i]) {
            r += (0 == i ? "" : ", ") + opds[i]->to_string();
        }
    }
    switch (op) {
        case CP:     return "CP    " + r;
        case ADD:    return "ADD   " + r;
        case MUL:    return "MUL   " + r;
        case DIV:    return "DIV   " + r;
        case SUB:    return "SUB   " + r;
        case CALL:   return "CALL  " + r;
        case PARAM:  return "PARAM " + r;
        case REF:    return "REF   " + r;
        case DEREF:  return "DEREF " + r;
        case LD:     return "LD    " + r;
        case GOTO:   return "GOTO  " + r;
        case IFEQ:   return "IFEQ  " + r;
        case IFNEQ:  return "IFNEQ " + r;
        case IFL:    return "IFLESS " + r;
        default:
            break;
    }
    
    return r;
}

unsigned int quad::get_goal_label(){
    assert (op == GOTO);
    return arg2->pint;
}
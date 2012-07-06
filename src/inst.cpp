#include "inst.h"

using namespace std;

opd::opd(symbol *s){
    data.sym = s;
    type = O_SYM;
}

opd::opd(){
    static int t = 0;
    data.pint = ++ t;
    type = O_TEMP;
}

opd::opd(int s, char type){

    data.pint = s;
    this->type = type;
    switch (type) {
        case O_LABEL: case O_INT:
            data.pint = s;
            break;
        case O_SL:
            data.spim.lab = s;
            break;
        case O_SI:
            data.spim.imm = s;
            break;
        default:
            throw string("") + "Error de implementación, tipo " + itoa_(type) + " de instrucción desconocido";
    }
}

opd::opd(int s1, int s2, char type){

    this->type = type;
    switch (type) {
        case O_SLI:
            data.spim.lab = s1;
            data.spim.imm = s2;
            break;
        case O_SLR:
            data.spim.lab = s1;
            data.spim.reg = s2;
            break;
        case O_SIR:
            data.spim.imm = s1;
            data.spim.reg = s2;
            break;
        default:
            throw string("") + "Error de implementación, tipo " + itoa_(type) + " de instrucción desconocido";
    }
}

opd::opd(int lab, int imm, int reg){

    this->type = O_SLIR;
    data.spim.lab = lab;
    data.spim.imm = imm;
    data.spim.reg = reg;
}

opd::opd(char c){
    data.pchar = c;
    type = O_CHAR;
}

opd::opd(bool s){
    data.pbool = s;
    type = O_BOOL;
}

opd::opd(float s){
    data.pfloat = s;
    type = O_FLOAT;
}

string opd::to_string() {
    stringstream out; 
    switch (type) {
        case O_SYM:   return data.sym->getName();
        case O_TEMP:  out << data.pint; return "T_" + out.str() ;
        case O_INT:   out << data.pint; return out.str();
        case O_CHAR:  return string(1, data.pchar);
        case O_BOOL:  return (data.pbool ? "true" : "false");
        case O_FLOAT: out << data.pfloat; return out.str();
        case O_LABEL: out << data.pint; return "L_" + out.str();
    }

    // registro de spim
    string r = "";
    if (type & O_SL) {
        stringstream out;
        out << data.spim.lab;
        r += out.str();
    }
    if (type & O_SI) {
        if (type & O_SL && data.spim.imm < 0) {
            r += "+";
        }
        stringstream out;
        out << data.spim.imm;
        r += out.str();
    }
    if (type & O_SR) {
        bool p = r.compare("");
        stringstream out;
        out << data.spim.reg;
        r += (p ? "(" : "") + out.str() + (p ? ")" : "");
    }

    return r.compare("") ? r : "UKNOWN_TYPE";
}

string inst::to_string(bool with_comment){
    return "";
}

unsigned int inst::get_goal_label(){
    assert (op == GOTO);
    if (0 != arg2) {
        return arg2->data.pint;
    } else {
        return -1;
    }
}

bool inst::is_jump(){
    switch ( op ){
        case GOTO:
        case IF:
        case IFEQ:
        case IFNEQ:
        case IFL:
        case IFLEQ:
        case IFG:
        case IFGEQ:
        case RETURN:
        case CALL:
        case EPILOGUE:
            return true;
        default:
            return false;
    }
}

bool inst::mandatory_jump(){
    assert(is_jump());
    switch ( op ){
        case GOTO:
//        case CALL:
        case RETURN:
        case EPILOGUE:
            return true;
        default:
            return false;
    }
}


quad::quad(OP op, opd *arg0, opd *arg1, opd *arg2, string comment){
    this->op = op;
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
    if (comment.compare("")) {
        comment = "# " + comment + "\n";
    }
    this->comment = comment;
}

string quad::to_string(bool with_comment){

    std::stringstream out;
    out << label;
    string r = "", _ = (with_comment ? comment : "" ) + "L" + out.str() + ": ";

    opd *opds[] = {arg0, arg1, arg2};
    
    // Modo alternativo de impresion: si es preferible el otro basta con remover
    // este segmento, me parece mas claro para Novich imprimir x := a + b que
    // ADD x a b, o x := a que CP x a.
    switch (op ){
        case CP:
            return _ + opds[0]->to_string() + " := " + opds[1]->to_string();
        case REF:
            return _ + opds[0]->to_string() + " := &" + opds[1]->to_string();
        case DEREF:
            return _ + opds[0]->to_string() + " := *" + opds[1]->to_string();
        case GOTO:
            return _ + "goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFEQ:
            return _ + "if " + opds[0]->to_string() + " = " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFL:
            return _ + "if " + opds[0]->to_string() + " < " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFLEQ:
            return _ + "if " + opds[0]->to_string() + " <= " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IF:
            return _ + "if " + opds[0]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFG:
            return _ + "if " + opds[0]->to_string() + " > " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case IFGEQ:
            return _ + "if " + opds[0]->to_string() + " >= " + opds[1]->to_string()
                    + " goto " + ( opds[2] ? opds[2]->to_string() : "unknown");
        case ADD:
            return _ + opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " + " + opds[2]->to_string();
        case SUB:
            return _ + opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " - " + opds[2]->to_string();
        case MUL:
            return _ + opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " * " + opds[2]->to_string();
        case DIV:
            return _ + opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " / " + opds[2]->to_string();
        case MOD:
            return _ + opds[0]->to_string() + " := " + opds[1]->to_string()
                    + " % " + opds[2]->to_string();
        case UMINUS:
            return _ + opds[0]->to_string() + " := -" + opds[1]->to_string();
        case PROLOGUE:
            return _ + "prologue " + opds[2]->data.sym->getName();
        case EPILOGUE:
            return _ + "epilogue " + opds[2]->data.sym->getName();
        case RETURN:
            return _ + "return " + opds[0]->to_string()
                     + " " + (opds[2] ? opds[2]->to_string() : "unkown");
        default:
            break;
    }
    
    for (int i = 0; i < 3; ++ i) {
        if (opds[i]) {
            r += (0 == i ? "" : ", ") + opds[i]->to_string();
        }
    }
    switch (op) {
        case CALL:   return _ + "CALL   " + r;
        case PARAM:  return _ + "PARAM  " + r;
        case LD:     return _ + "LD     " + r;
        case ST:     return _ + "ST     " + r;
        case ERR:    return _ + "ERR    " + r;
        case CONV:   return _ + "CONV   " + r;
        case GOTO:   return _ + "GOTO   " + r;
        case IFEQ:   return _ + "IFEQ   " + r;
        case IFNEQ:  return _ + "IFNEQ  " + r;
        case IFL:    return _ + "IFLESS " + r;
        case INIT:   return _ + "INIT   " + r;
        case WRITE:  return _ + "WRITE  " + r;
        case READ:   return _ + "READ   " + r;
        case MAP:    return _ + "MAP    " + r;
        case FILL:   return _ + "FILL   " + r;
        case EXEC:   return _ + "EXEC   " + r;

        default:
            break;
    }
    
    return r;
}

Spim_Inst::Spim_Inst(OP op, opd *arg0, opd *arg1, opd *arg2, string comment){
    this->op = op;
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
    if (comment.compare("")) {
        comment = "# " + comment + "\n";
    }
    this->comment = comment;
}

string Spim_Inst::to_string(bool with_comment){

    std::stringstream out;
    out << label;
    string r = "", _ = (with_comment ? comment : "" ) + "L" + out.str() + ": ";
    
    opd *opds[] = {arg0, arg1, arg2};
    
    switch (op ){
        default:
            break;
    }
    
    for (int i = 0; i < 3; ++ i) {
        if (opds[i]) {
            r += (0 == i ? "" : ", ") + opds[i]->to_string();
        }
    }
    
    switch (op ){
        default:
            break;
    }
    
    return r;
}
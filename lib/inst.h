#ifndef INST_H
#define INST_H

#include "utils.h"
#include "codegen.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <string>

#define O_SYM   0
#define O_TEMP  1
#define O_INT   2
#define O_CHAR  3
#define O_FLOAT 4
#define O_BOOL  5
#define O_LABEL 6

#define O_SL    8
#define O_SI   16
#define O_SLI  24
#define O_SR   32
#define O_SLR  40
#define O_SIR  48
#define O_SLIR 56

#include "symbol.h"

struct opd{
    union {
        symbol *sym;

        int pint;
        char pchar;
        bool pbool;
        float pfloat;
        struct {
            int lab;
            int imm;
            int reg;
        } spim;
    } data;
    char type;

    opd(symbol *s);
    opd();
    opd(int s1, char type = O_INT);
    opd(int s1, int s2, char type);
    opd(int lab, int imm, int reg);
    opd(char s);
    opd(bool s);
    opd(float s);
    
    string to_string();
};


class inst{
    public:
                // particulares del tac
        enum OP {CP, CALL, PARAM, REF, DEREF, ERR, CONV,
                 MOD, EXEC, INIT, WRITE, READ, MAP, FILL,
                 PROLOGUE, EPILOGUE, RETURN,

                 // tac y spim
                 ADD, SUB, MUL, DIV, UMINUS, GOTO,
                 IF, IFEQ, IFNEQ, IFL, IFLEQ, IFG, IFGEQ,

                 // particulares de spim
                 SYSCALL, LI, LA, LD, ST};
        
        unsigned int label;
        
        OP op;
        
        opd *arg0;
        opd *arg1;
        opd *arg2;
        string comment;
        
        unsigned get_goal_label();
        
        virtual string to_string(bool with_comment = true);
        
        virtual bool is_jump();
        virtual bool mandatory_jump();

        string to_string();
};

class quad : public inst{
    public:
        quad(OP op, opd *arg0, opd *arg1 = 0, opd *arg2 = 0, string comment = "");

        virtual string to_string(bool with_comment = false);
};

class Spim_Inst : public inst{
    public:
        Spim_Inst(OP op, opd *arg0, opd *arg1 = 0, opd *arg2 = 0, string comment = "");

        virtual string to_string(bool with_comment = false);
};

#endif
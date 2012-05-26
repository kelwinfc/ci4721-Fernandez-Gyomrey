#ifndef _INST
#define _INST

#include <sstream>
#include <cstdio>

#define O_SYM   0
#define O_TEMP  1
#define O_INT   2
#define O_CHAR  3
#define O_FLOAT 4
#define O_BOOL  5
#define O_LABEL 6

#include "symbol.h"

struct opd{
    symbol *sym;
    int temp;
    int pint;
    char pchar;
    bool pbool;
    float pfloat;
    char type;
    
    opd(symbol *s);
    opd();
    opd(int s, int label=0 );
    opd(char s);
    opd(bool s);
    opd(float s);
    
    string to_string();
};

class inst{
    public:
        
        unsigned int label;
        virtual string to_string();
        
};

class quad : public inst{
    public:
        enum OP {CP, CALL, PARAM, REF, LD,
                 ADD, SUB, MUL, DIV, MOD, UMINUS,
                 GOTO,
                 IF, IFEQ, IFNEQ, IFL, IFLEQ, IFG, IFGEQ};
        OP op;
        
        opd *arg0;
        opd *arg1;
        opd *arg2;

    public:
        quad(OP op, opd *arg0, opd *arg1 = 0, opd *arg2 = 0);

        string to_string();
        
        unsigned get_goal_label();
};

class mi : public inst{

};

#endif
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

#include "symbol.h"

int newtemp();

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
    opd(int s);
    opd(char s);
    opd(bool s);
    opd(float s);

    string to_string();
};

class inst{
    public:
        string to_string();
};

class quad : public inst{
    public:
        enum OP {CP, ADD, CALL, PARAM, ACCESS, REF, DEREF, LD};

    private:
        OP op;
        opd *arg0;
        opd *arg1;
        opd *arg2;

    public:
        quad(OP op, opd *arg0, opd *arg1 = 0, opd *arg2 = 0);

        string to_string();
};

class mi : public inst{

};

#endif
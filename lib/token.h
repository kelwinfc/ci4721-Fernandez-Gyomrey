#ifndef TOKEN_H
#define TOKEN_H

#include <cstring>
#include <cstdlib>
#include <iostream>
#include "utils.h"
#include "llog.h"

using namespace std;

extern llog* logger;

class token{
    public:

        int line;

        int column;

        token();

        token(int l, int c);
};

class tokenId : public token{
    public:

        string ident;

        tokenId(int l, int c, char* id);
        
        ~tokenId(){ ident.clear(); }
};

class tokenInt : public token {
    public:

        int number;

        tokenInt(int l, int c, char* num);
};

class tokenFloat : public token {
    public:

        float number;

        tokenFloat(int l, int c, char* num);
};

class tokenBoolean : public token {
    public:

        bool value;

        tokenBoolean(int l, int c, char* val);
};

class tokenConstant : public token {
    public:

        string ident;

        int type;

        tokenConstant(int l, int c, char* val, int t);
};

#endif
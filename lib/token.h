#ifndef __TOKEN
#define __TOKEN

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "utils.h"

using namespace std;

class token{
    public:

        int line;

        token(int l = 0);
};

class tokenId : public token{
    public:

        char* ident;

        tokenId(int l, char* id);
};

class tokenType : public token {
    private:

        enum TYPE { INT, FLOAT, CHAR, BOOLEAN };

    public:

        TYPE ident;

        tokenType(int l, char* id);

        void print();
        
        char* type(){
            switch ( ident ) {
                case INT:
                    return (char*)"int";
                case FLOAT:
                    return (char*)"float";
                case CHAR:
                    return (char*)"char";
                case BOOLEAN:
                    return (char*)"boolean";
            }
            
            return (char*)"";
        }
};

class tokenInt : public token {
    public:

        int number;

        tokenInt(int l, char* num);
};

class tokenFloat : public token {
    public:

        float number;

        tokenFloat(int l, char* num);
};

class tokenBoolean : public token {
    public:

        bool value;

        tokenBoolean(int l, char* val);
};

#endif
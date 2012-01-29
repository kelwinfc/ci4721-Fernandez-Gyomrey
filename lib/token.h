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
        
        token(){
            line = 0;
        }
        
        token(int l){
            line = l;
        }
};

class tokenId : public token {
    
    public:
        char* ident;
        
        tokenId(int l, char* id);
};

class tokenType : public token {
    
    enum TYPE { INT, FLOAT, CHAR, BOOLEAN };
    
    public:
        TYPE ident;
        
        tokenType(int l, char* id);
        
        void print();
};

class tokenInt : public token {
    public:
        int number;
        
        tokenInt(int l, char* num);
};

class tokenFloat : public token {
    public:
        float number;
        
        tokenFloat(int l, char* num){
            line = l;
            number = atof(num);
        }
};

class tokenBoolean : public token {
    public:
        bool value;
        
        tokenBoolean(int l, char* val){
            line = l;
            
            if ( strcmp(val,"true") == 0 ){
                value = 1;
            } else {
                value = 0;
            }
        }
};

#endif
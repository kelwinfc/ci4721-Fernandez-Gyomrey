#ifndef __TOKEN
#define __TOKEN

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

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
        
        tokenType(int l, char* id){
            line = l;
            
            if ( strcmp(id, "int") == 0 ){
                ident = INT;
            } else if ( strcmp(id, "float") == 0 ){
                ident = FLOAT;
            } else if ( strcmp(id, "char") == 0 ){
                ident = CHAR;
            } else if ( strcmp(id, "boolean") == 0 ){
                ident = BOOLEAN;
            } else {
                
            }
        }
};

class tokenInt : public token {
    public:
        int number;
        
        tokenInt(int l, char* num){
            line = l;
            
            int length = strlen(num);
            bool has_underscore = false;
            for (int i=0; i<length; i++){
                if ( num[i] == '_' ){
                    has_underscore = true;
                    break;
                }
            }
            
            if ( has_underscore ){
                int i=length-1;
                int base = 0;
                int pow = 1;
                
                while( num[i] != '_' ){
                    base += pow*(num[i] - '0');
                    pow *= 10;
                    i--;
                }
                
                number = 0;
                pow = 1;
                
                if ( base > 36 || base == 0 ){
                    number = -1;
                } else if ( base != 1 ){
                    i--;
                    while( i >= 0 ){
                        int diff = 0;
                        if ( '0' <= num[i] && num[i] <= '9' ){
                            diff = num[i] - '0';
                        } else if ( 'A' <= num[i] && num[i] <= 'Z' ){
                            diff = num[i] - 'A';
                        } else {
                            diff = num[i] - 'a';
                        }
                        
                        if ( diff >= base ){
                            number = -1;
                            break;
                        } else {
                            number += pow*diff;
                        }
                        pow *= base;
                        i--;
                    }
                } else {
                    bool check = true;
                    for (int j=0; j<i; j++){
                        check = check && ( num[j] == '0' );
                    }
                    number = i;
                }
                
            } else {
                number = atoi (num);
            }
        }
};

#endif
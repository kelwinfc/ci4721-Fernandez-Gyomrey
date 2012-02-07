#include "token.h"

using namespace std;

token::token(int l){
    line = l;
}

tokenFloat::tokenFloat(int l, char* num){
    line = l;
    number = atof(num);
}

tokenBoolean::tokenBoolean(int l, char* val){
    line = l;
    
    if ( strcmp(val,"true") == 0 ){
        value = true;
    } else {
        value = false;
    }
}

tokenId::tokenId(int l, char* id){
    line = l;
    
    ident = string(id);
}

tokenType::tokenType(int l, char* id){
    
    line = l;
    if ( strcmp(id, "int") == 0 ){
        ident = symbol::INT;
    } else if ( strcmp(id, "float") == 0 ){
        ident = symbol::FLOAT;
    } else if ( strcmp(id, "char") == 0 ){
        ident = symbol::CHAR;
    } else if ( strcmp(id, "boolean") == 0 ){
        ident = symbol::BOOLEAN;
    } else {
        throw "El tipo no existe";
    }
}

tokenInt::tokenInt(int l, char* num){
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

void tokenType::print(){
    if ( ident == symbol::INT ){
        cout << "int";
    } else if ( ident == symbol::FLOAT ){
        cout << "float";
    } else if ( ident == symbol::BOOLEAN ){
        cout << "boolean";
    } else if ( ident == symbol::CHAR ){
        cout << "char";
    } else {
        
    }
}
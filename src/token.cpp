#include "token.h"

tokenId::tokenId(int l, char* id)
{
    line = l;
    
    ident = (char*)malloc( (strlen(id)+1)*sizeof(char) );
    memset(ident, 0, sizeof ident);
    sprintf(ident, "%s", id );
}

tokenType::tokenType(int l, char* id){
    
    line = l;
    
    if ( strcmp(id, "int") == 0 ){
        ident = tokenType::INT;
    } else if ( strcmp(id, "float") == 0 ){
        ident = tokenType::FLOAT;
    } else if ( strcmp(id, "char") == 0 ){
        ident = tokenType::CHAR;
    } else if ( strcmp(id, "boolean") == 0 ){
        ident = tokenType::BOOLEAN;
    } else {
        
    }
}

void tokenType::print(){
    if ( ident == tokenType::INT ){
        cout << "int";
    } else if ( ident == tokenType::FLOAT ){
        cout << "float";
    } else if ( ident == tokenType::BOOLEAN ){
        cout << "boolean";
    } else if ( ident == tokenType::CHAR ){
        cout << "char";
    } else {
        
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
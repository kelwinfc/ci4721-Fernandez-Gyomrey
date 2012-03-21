#include "token.h"

using namespace std;

token::token(){}

token::token(int l, int c){
    line = l;
    column = c;
}

tokenFloat::tokenFloat(int l, int c, char* num){
    line = l;
    column = c;
    number = atof(num);
}

tokenBoolean::tokenBoolean(int l, int c, char* val){
    line = l;
    column = c;
    
    if ( strcmp(val,"true") == 0 ){
        value = true;
    } else {
        value = false;
    }
}

tokenId::tokenId(int l, int c, char* id){
    line = l;
    column = c;
    
    ident = string(id);
}

tokenInt::tokenInt(int l, int c, char* num){
    line = l;
    column = c;
    
    int length = strlen(num);
    bool has_underscore = false;
    for (int i=0; i<length; i++){
        if ( num[i] == '_' ){
            has_underscore = true;
            break;
        }
    }
    
    // Previene overflow en el numero
    unsigned long long int number_long = 0;
    
    if ( has_underscore ){
        int i=length-1;
        int base = 0;
        int pow = 1;
        
        while( num[i] != '_' ){
            base += pow*(num[i] - '0');
            pow *= 10;
            i--;
            
            // Previene overflow en la base
            if ( base > 36 ){
                break;
            }
        }
        
        number = 0;
        pow = 1;
        
        if ( base > 36 || base == 0 ){
            logger->error(line, column, "La base debe estar comprendida en el intervalo [0,36]");
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
                    number_long += pow*diff;
                    
                    if ( number_long > (long long int)2147483647LL ){
                        break;
                    }
                }
                pow *= base;
                i--;
            }
        } else {
            bool check = true;
            for (int j=0; j<i; j++){
                check = check && ( num[j] == '0' );
            }
            number_long = i;
        }
        
    } else {
        
        int i=length-1;
        int pow=1;
        while( i >= 0 ){
            int diff = 0;
            if ( '0' <= num[i] && num[i] <= '9' ){
                diff = num[i] - '0';
            }
            
            number_long += pow*diff;
            
            if ( number_long > (long long int)2147483647LL ){
                break;
            }
            
            pow *= 10;
            i--;
        }
    }
    
    if ( number != -1 && number_long > (long long int)2147483647LL )
    {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Constante entera '%s' muy grande.", num);
        logger->error(line, column, e);
        
        number = -1;
        
    } else if ( number != -1 ){
        number = (int)number_long;
    }
}

tokenConstant::tokenConstant(int l, int c, char* id, int t){
    line = l;
    column = c;
    
    ident = string(id);
    type = t;
}

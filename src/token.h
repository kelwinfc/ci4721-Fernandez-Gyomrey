#ifndef __TOKEN
#define __TOKEN

#include <cstring>
#include <cstdlib>
#include <cstdio>

class token{
    public:
        int line;
        int col;
        
        token(){
            line = col = 0;
        }
        
        token(int l, int c){
            line = l;
            col = c;
        }
};

class tokenId : public token {
    
    public:
        char* ident;
        
        tokenId(int l, int c, char* id){
            line = l;
            col = c;
            
            ident = (char*)malloc( (strlen(id)+1)*sizeof(char) );
            memset(ident, 0, sizeof ident);
            sprintf(ident, "%s", id );
        }
};

#endif
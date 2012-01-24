#include "token.h"

tokenId::tokenId(int l, char* id)
{
    line = l;
    
    ident = (char*)malloc( (strlen(id)+1)*sizeof(char) );
    memset(ident, 0, sizeof ident);
    sprintf(ident, "%s", id );
}

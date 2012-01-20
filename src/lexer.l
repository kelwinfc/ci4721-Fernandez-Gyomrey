%{
#include <math.h>
#include "token.h"
#include "y.tab.h"

%}

DIGIT    [0-9]
ID       [a-z][a-z0-9]*
     
%%
  
[ \t]                       /* Ignore whitespace */
--[^\n]*\n                  /* Ignore line comment */
"{-"[^\0]*"-}"              /* Ignore block comment */

";"|"="|"("|")"|"{"|"}"|","     { return yytext[0]; }

"int"|"float"|"char"|"boolean"|"none"   { return TK_TYPE;   }
"fun"                                   { return TK_FUNCTION; }

{ID}                            { yylval.tk = new tokenId(2,3,yytext); return TK_IDENT; }

{DIGIT}+ { return TK_INT; }

%%
     
int yywrap() { return 0; }

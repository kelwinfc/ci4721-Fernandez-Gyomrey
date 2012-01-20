%{
#include <math.h>
#include "token.h"
#include "y.tab.h"

class AA {
public:
    int a;
    
    AA(){ a=0; }
};

YYSTYPE = AA

%}
     
DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*

%option c++

%%

[ \t]                  /* Ignore whitespace */
--[^\n]*\n             /* Line comment */
"{-"[^\0]*"-}"         /* Block comment */

{ID}                                {
                                        yylval.line = 0;
                                        return IDENT;
                                    }

{DIGIT}*_{DIGIT}{DIGIT}?            {
                                        return NUM;
                                    }

{DIGIT}+               {
                           sscanf(yytext,"%lf",&yylval); return NUM;
                       }

{DIGIT}+("."{DIGIT}*)? { sscanf(yytext,"%lf",&yylval); return NUM; }
"+"|"-"|"*"|"/"|"^"|\n return yytext[0];

%%

int yywrap() { }

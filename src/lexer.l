%{
#include <math.h>
#include "lib/AST.h"
#include "y.tab.h"

extern bool error;

int columnline = 0, column = 0, lastlength = 0;

void updatecol(){
    column += lastlength;
    if (columnline != yylineno){
        columnline = yylineno;
        column = 0;
    }
    lastlength = yyleng;
}

%}

DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*

%option yylineno

%%

<<EOF>>                             {   updatecol();
                                        yyterminate();
                                        return TK_END_OF_FILE;
                                    }

[ \t\r\n]                                             /* Ignore whitespace */{ updatecol(); }
--[^\r\n]*                                            /* Ignore line comment */{ updatecol(); }
"{-"([^-]|[\r\n]|(-+([^}]|\r\n])))*-+"}"              /* Ignore block comment */{ updatecol(); }

";"|"="|"("|")"|"{"|"}"|"," { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return yytext[0];
                            }

"+"|"-"|"*"|"/"             { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return yytext[0];
                            }

"&&"                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_AND;
                            }

"||"                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_OR;
                            }

"==>"                       { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_IMP;
                            }

"<=="                       { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_CONSEQ;
                            }

"=="                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_EQ;
                            }

"!="                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_UNEQ;
                            }

"!"                         { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_NOT;
                            }

"<"                         { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_LESS;
                            }

"<="                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_LESS_EQ;
                            }

">"                         { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_GREAT;
                            }

">="                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_GREAT_EQ;
                            }

"int"|"float"|"char"|"boolean" { updatecol();
                                 yylval.tk = new tokenType(yylineno, column, yytext);
                                 return TK_TYPE;
                               }

"true"|"false"                 { updatecol();
                                 yylval.tk = new tokenBoolean(yylineno, column, yytext);
                                 return TK_BOOLEAN;
                               }

'.?'|'"\\".'                   { updatecol();
                                 yylval.tk = new tokenId(yylineno, column, yytext);
                                 return TK_CHAR;
                               }

"none"                         { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_NONE;
                               }

"fun"                          { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_FUNCTION;
                               }

if                             { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_IF;
                               }

elif                           { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_ELIF;
                               }

else                           { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_ELSE;
                               }

while                          { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_WHILE;
                               }

for                            { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_FOR;
                               }

in                             { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_IN;
                               }
                               
break                          { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_BREAK;
                               }

continue                       { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_CONTINUE;
                               }

return                         { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_RETURN;
                               }

[0-9a-zA-Z]*"_"{DIGIT}+         { updatecol();
                                  yylval.tk = new tokenInt(yylineno, column, yytext);
                                  if ( ((tokenInt*)yylval.tk)->number == -1 ){
                                      fprintf(stderr,
                                              "Line %d: Invalid constant %s.\n",
                                              yylineno,
                                              yytext
                                             );
                                      error = true;
                                  }
                                  return TK_INT;
                                }

{DIGIT}+\.{DIGIT}*              { updatecol();
                                  yylval.tk = new tokenFloat(yylineno, column, yytext);
                                  return TK_FLOAT;
                                }

{DIGIT}+                        { updatecol();
                                  yylval.tk = new tokenInt(yylineno, column, yytext);
                                  return TK_INT;
                                }

{ID}                            { updatecol();
                                  yylval.tk = new tokenId(yylineno, column, yytext);
                                  return TK_IDENT;
                                }

%%
     
int yywrap() { return 1; }

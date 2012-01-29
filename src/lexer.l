%{
#include <math.h>
#include "lib/AST.h"
#include "y.tab.h"

extern bool error;

%}

DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*

%option yylineno

%%

<<EOF>>                             {
                                        yyterminate();
                                        return TK_END_OF_FILE;
                                    }

[ \t\r\n]                                             /* Ignore whitespace */
--[^\r\n]*                                            /* Ignore line comment */
"{-"([^-]|[\r\n]|(-+([^}]|\r\n])))*-+"}"              /* Ignore block comment */

";"|"="|"("|")"|"{"|"}"|"," { yylval.tk = new token(yylineno);
                              return yytext[0];
                            }

"+"|"-"|"*"|"/"             {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return yytext[0];
                            }

"&&"                        {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_AND;
                            }

"||"                        {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_OR;
                            }

"==>"                       {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_IMP;
                            }

"<=="                       {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_CONSEQ;
                            }

"=="                        {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_EQ;
                            }

"!="                        {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_UNEQ;
                            }

"!"                         {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_NOT;
                            }

"<"                         {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_LESS;
                            }

"<="                        {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_LESS_EQ;
                            }

">"                         {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_GREAT;
                            }

">="                        {
                              yylval.tk = new tokenId(yylineno, yytext);
                              return TK_GREAT_EQ;
                            }

"int"|"float"|"char"|"boolean" { yylval.tk = new tokenType(yylineno, yytext);
                                 return TK_TYPE;
                               }

"true"|"false"                 { yylval.tk = new tokenBoolean(yylineno, yytext);
                                 return TK_BOOLEAN;
                               }

'.?'|'"\\".'                   {
                                 yylval.tk = new tokenId(yylineno, yytext);
                                 return TK_CHAR;
                               }

"none"                         { //yylval.tk = new token(yylineno);
                                 return TK_NONE;
                               }

"fun"                          { yylval.tk = new token(yylineno);
                                 return TK_FUNCTION;
                               }

if                             {
                                 yylval.tk = new token(yylineno);
                                 return TK_IF;
                               }

elif                           {
                                 yylval.tk = new token(yylineno);
                                 return TK_ELIF;
                               }

else                           {
                                 yylval.tk = new token(yylineno);
                                 return TK_ELSE;
                               }

while                          {
                                 yylval.tk = new token(yylineno);
                                 return TK_WHILE;
                               }

for                            {
                                 yylval.tk = new token(yylineno);
                                 return TK_FOR;
                               }

in                             {
                                 yylval.tk = new token(yylineno);
                                 return TK_IN;
                               }
                               
break                          {
                                 yylval.tk = new token(yylineno);
                                 return TK_BREAK;
                               }

continue                       {
                                 yylval.tk = new token(yylineno);
                                 return TK_CONTINUE;
                               }

return                         {
                                 yylval.tk = new token(yylineno);
                                 return TK_RETURN;
                               }

[0-9a-zA-Z]*"_"{DIGIT}+         { yylval.tk = new tokenInt(yylineno, yytext);
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

{DIGIT}+\.{DIGIT}*              { yylval.tk = new tokenFloat(yylineno, yytext);
                                  return TK_FLOAT;
                                }

{DIGIT}+                        { yylval.tk = new tokenInt(yylineno, yytext);
                                  return TK_INT;
                                }

{ID}                            { yylval.tk = new tokenId(yylineno, yytext);
                                  return TK_IDENT;
                                }

%%
     
int yywrap() { return 1; }

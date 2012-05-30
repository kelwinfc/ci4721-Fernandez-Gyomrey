%{
#include <math.h>
#include "lib/AST.h"
#include "lib/llog.h"
#include "y.tab.h"

extern llog* logger;
extern map<string, int> constants;

int columnline = 0, column = 0, lastlength = 0;

void updatecol(bool containsLinebreaks = false){
    column += lastlength;
    lastlength = yyleng;
    if (containsLinebreaks){
        int i = yyleng;
        while (0 < i && yytext[i - 1] != '\n') --i;
        lastlength = yyleng - i;
    }
    if (columnline != yylineno){
        columnline = yylineno;
        column = 1;
    }
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

[ \t\r\n]                                             { updatecol(true); }
--[^\r\n]*                                            { updatecol(true); }
"{-"([^-]|[\r\n]|(-+([^}]|\r\n])))*-+"}"              { updatecol(true); }

";"|"="|"("|")"|"{"|"}"|"," { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return yytext[0];
                            }

"^"                         { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_POINTER;
                            }

"@"                         { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_ADDRESS;
                            }

"%"                         { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_MOD;
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

"<="                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_LESS_EQ;
                            }

"<"                         { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_LESS;
                            }

">="                        { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_GREAT_EQ;
                            }

">"                         { updatecol();
                              yylval.tk = new tokenId(yylineno, column, yytext);
                              return TK_GREAT;
                            }

"alias"                     { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_ALIAS;
                            }

"type"                      { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_NEW_TYPE;
                            }

"union"                     { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_UNION;
                            }

"as"                        { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_AS;
                            }

"fill"                      { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_FILL;
                            }

"with"                      { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_WITH;
                            }

"map"                       { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_MAP;
                            }

"remember"                  { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_REMEMBER;
                            }

"forget"                    { updatecol();
                              yylval.tk = new token(yylineno, column);
                              return TK_FORGET;
                            }

"foldr"|"foldl"             { updatecol();
                              fprintf(stderr,
                                      "Warning: Palabra reservada %s aún "
                                      "sin implementarse.\n",
                                      yytext);
                            }

"["|"]"|":"|"."                { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return yytext[0];
                               }

"read"                         { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_READ;
                               }

"print"                        { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_PRINT;
                               }

"const"                        { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_CONST;
                               }

"int"|"float"|"char"|"string"|"boolean" { updatecol();
                                 yylval.tk = new tokenId(yylineno, column, yytext);
                                 return TK_IDENT;
                               }

"true"|"false"                 { updatecol();
                                 yylval.tk = new tokenBoolean(yylineno, column, yytext);
                                 return TK_BOOLEAN;
                               }

'.?'|'"\\".'                   { updatecol();
                                 yylval.tk = new tokenId(yylineno, column, yytext);
                                 return TK_CHAR;
                               }
L?\"(\\.|[^\\"])*\"            {
                                 updatecol();
                                 yylval.tk = new tokenId(yylineno, column, yytext);
                                 return TK_STRING;
                               }

"enum"                         { updatecol();
                                 yylval.tk = new token(yylineno, column);
                                 return TK_ENUM;
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
                                  return TK_INT;
                                }

{DIGIT}+\.{DIGIT}*              { updatecol();
                                  yylval.tk = new tokenFloat(yylineno, column, yytext);
                                  return TK_FLOAT;
                                }

{DIGIT}+(\.{DIGIT}*)?[eE]([-\+]?[0-9]+)? { updatecol();
                                           yylval.tk = new tokenFloat(yylineno, column, yytext);
                                           return TK_FLOAT;
                                         }

{DIGIT}+                        { updatecol();
                                  yylval.tk = new tokenInt(yylineno, column, yytext);
                                  return TK_INT;
                                }

{ID}                            { updatecol();
                                  map<string, int>::iterator it = constants.find(yytext);
                                  if (it != constants.end()) {
                                      yylval.tk = new tokenConstant(yylineno, column, yytext, (*it).second);
                                      return TK_ENUM_CONSTANT;
                                  } else {
                                      yylval.tk = new tokenId(yylineno, column, yytext);
                                      return TK_IDENT;
                                  }
                                }

%%
     
int yywrap() { return 1; }

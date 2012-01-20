%{

#include <math.h>
#include <stdio.h>
#include "token.h"

int yylex (void);
void yyerror (char const *);

%}

%union{
    token *tk;
}

/* Bison declarations.  */
%token TK_INT TK_IDENT TK_TYPE TK_TYPE_FLOAT TK_FUNCTION

%% /* The grammar follows.  */
input : global_declaration 
      | global_declaration input
      ;

global_declaration : function_declaration
                   | declaration
                   ;

declaration : TK_TYPE TK_IDENT ';'               {printf("hey");}
            | TK_TYPE TK_IDENT '=' EXPR ';'      {printf("hey con init");}
            ;

function_declaration : TK_FUNCTION TK_TYPE TK_IDENT 
                            '(' argument_list ')'
                            '{'    '}' {printf("function");}

argument_list : /*empty */
              | non_empty_argument_list
              ;

non_empty_argument_list : TK_TYPE TK_IDENT
                        | TK_TYPE TK_IDENT ',' non_empty_argument_list

EXPR : TK_INT           { ; }
     | TK_IDENT         { ; }
     ;

%%

void yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}

int main (int argc,char **argv)
{
  yyparse ();
  return 0;
}

%{

#include <math.h>
#include <stdio.h>
#include "lib/AST.h"
#include "lib/symbol.h"
#include "lib/symbol_table.h"

int yylex (void);
void yyerror (char const *);

extern FILE *yyin;

AST_program * p;
bool error = false;

%}

%union{
    AST_node* nd;
    token* tk;
}

/* Bison declarations.  */
%token TK_INT TK_FLOAT TK_BOOLEAN TK_CHAR 
%token TK_IDENT
%token TK_TYPE
%token TK_FUNCTION
%token TK_NONE
%token TK_END
%token TK_END_OF_FILE 0
%token TK_IF TK_ELSE TK_ELIF
%token TK_WHILE TK_FOR TK_IN TK_BREAK TK_CONTINUE TK_RETURN
%token TK_CONST

%token TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
%token TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ

%left TK_AND TK_OR
%right TK_IMP
%left TK_CONSEQ
%left TK_EQ TK_UNEQ
%nonassoc TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ
%left '-' '+'
%left '*' '/'
%left NEG


%type<nd> input declaration variable_declaration block statement
          parameters_instance parameters_instance_non_empty
          arg_list non_empty_arg_list
          block_statement loop_statement loop_block loop_block_statement
          else_statements expression aritmetic_expression boolean_expression
%type<tk> TK_TYPE TK_IDENT TK_FUNCTION TK_NONE TK_IF TK_ELSE
          TK_CONST TK_INT TK_FLOAT TK_BOOLEAN TK_CHAR
          TK_BREAK TK_CONTINUE TK_RETURN TK_ELIF TK_WHILE
          TK_FOR TK_IN TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
          TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ
          '(' ')' '+' '-' '*' '/'

%%

// Definicion de un programa
input:
        declaration       { p->add_declaration((AST_declaration*)$1); }
    |   input declaration { p->add_declaration((AST_declaration*)$2); }
;

// Declaracion de variables y funciones
declaration:
        variable_declaration                               
            { $$ = $1 }
    |   TK_FUNCTION TK_TYPE TK_IDENT '(' arg_list ')' block
            { $$ = new AST_function( (tokenType*)$2,
                                     (tokenId*)$3,
                                     (AST_arg_list*)$5,
                                     (AST_block*)$7
                                   );
            }
    |   TK_FUNCTION TK_NONE TK_IDENT '(' arg_list ')' block
            { $$ = new AST_function( 0,
                                     (tokenId*)$3,
                                     (AST_arg_list*)$5,
                                     (AST_block*)$7
                                   );
            }
;

// Declaracion de variables
variable_declaration :
      TK_CONST TK_TYPE TK_IDENT '=' expression ';'
            { $$ = new AST_variable_declaration( (tokenType*)$2,
                                                 (tokenId*)$3,
                                                 (AST_expression*)$5,
                                                 true
                                               );
            }
    | TK_TYPE TK_IDENT '=' expression ';'
            { $$ = new AST_variable_declaration( (tokenType*)$1,
                                                 (tokenId*)$2,
                                                 (AST_expression*)$4
                                               );
            }
    |   TK_TYPE TK_IDENT ';'
            { $$ = new AST_variable_declaration( (tokenType*)$1,
                                                 (tokenId*)$2,
                                                 0
                                               );
            }
;

// Lista de parámetros formales (puede ser vacía)
arg_list:
      /* empty */        { $$ = new AST_arg_list(); }
    | non_empty_arg_list { $$ = $1; }
;

// Lista de parámetros que no puede ser vacía
non_empty_arg_list :
        TK_CONST TK_TYPE TK_IDENT
            {
                AST_arg_list* ar = new AST_arg_list();
                ar->add_argument( (tokenType*)$2,
                                  (tokenId*)$3,
                                  true
                                );
                $$ = ar;
            }
    |   TK_TYPE TK_IDENT
            {
                AST_arg_list* ar = new AST_arg_list();
                ar->add_argument( (tokenType*)$1,
                                  (tokenId*)$2
                                );
                $$ = ar;
            }
    |   non_empty_arg_list ',' TK_TYPE TK_IDENT
            {
                ((AST_arg_list*)$1)->add_argument( (tokenType*)$3,
                                                   (tokenId*)$4
                                                 );
                $$ = $1;
            }
    |   non_empty_arg_list ',' TK_CONST TK_TYPE TK_IDENT
            {
                ((AST_arg_list*)$1)->add_argument( (tokenType*)$4,
                                                   (tokenId*)$5,
                                                   true
                                                 );
                $$ = $1;
            }
;

// Definicion de bloques de anidamiento
block :
        '{' block_statement '}' { $$ = $2 }
    |   '{' '}'                 { $$ = new AST_block(); }
;

// Bloques de instrucciones
block_statement :
        statement
            {
                AST_block* b = new AST_block();
                if ( $1 != 0 )
                    b->add_statement( (AST_statement*)$1 );
                $$ = b;
            }
    |   block_statement statement
            {
                if ( $2 != 0 )
                    ((AST_block*)$1)->add_statement((AST_statement*)$2);
                $$ = $1;
            }
;

// Instrucciones
statement :
        variable_declaration
            { $$ = $1; }
    |   TK_IDENT '=' expression ';'
            { $$ = new AST_assignment((tokenId*)$1, (AST_expression*)$3); }
    |   ';'
            { $$ = 0  }
    |   TK_IF '(' expression ')' block else_statements 
            { $$ = new AST_conditional( $1,
                                        (AST_expression*) $3,
                                        (AST_block*) $5,
                                        (AST_conditional*) $6
                                      );
            }
    |   TK_WHILE '(' expression ')' loop_block
            { $$ = new AST_loop( (token*)$1,
                                 (AST_expression*)$3,
                                 (AST_block*)$5
                               );
            }
    |   TK_FOR TK_IDENT TK_IN '(' expression ',' expression ')' loop_block
            { $$ = new AST_bounded_loop( (token*)$1,
                                         (tokenId*)$2,
                                         (AST_expression*)$5,
                                         (AST_expression*)$7,
                                         (AST_block*)$9);
            }
    |   block
            { $$ = $1 }
    |   TK_RETURN ';'
            { $$ = new AST_return($1, 0); }
    |   TK_RETURN expression ';'
            { $$ = new AST_return($1,(AST_expression*)$2); }
;

// Loop blocks: includes break and continue to statements
loop_block:
        '{' loop_block_statement '}' { $$ = $2 }
    |   '{' '}'                      { $$ = new AST_block(); }
;

loop_block_statement:
        loop_statement
            {
                AST_block* b = new AST_block();
                if ( $1 != 0 )
                    b->add_statement((AST_statement*)$1);
                $$ = b;
            }
    |   loop_statement loop_block_statement
            {
                if ( $1 != 0 )
                    ((AST_block*)$2)->add_statement((AST_statement*)$1);
                $$ = $2;
            }
;

loop_statement :
        statement       { $$ = $1                    }
    |   TK_BREAK ';'    { $$ = new AST_break($1);    }
    |   TK_CONTINUE ';' { $$ = new AST_continue($1); }
;

else_statements :
            { $$ = 0 }
    |   TK_ELIF '(' expression ')' block else_statements
            { $$ = new AST_conditional( $1,
                                        (AST_expression*) $3,
                                        (AST_block*) $5,
                                        (AST_conditional*) $6
                                      );
            }
    |   TK_ELSE block
            { $$ = new AST_conditional($1, 0, (AST_block*)$2, 0 ); }
;

// Subgramatica de expresiones
expression:
        TK_INT               { $$ = new AST_int( (tokenInt*)$1); }
    |   TK_FLOAT             { $$ = new AST_float( (tokenFloat*)$1); }
    |   TK_BOOLEAN           { $$ = new AST_boolean( (tokenBoolean*)$1); }
    |   TK_CHAR              { $$ = new AST_char( (tokenId*)$1); }
    |   '(' expression ')'   { $$ = (AST_expression*)$2; }
    |   TK_IDENT             { $$ = new AST_ident((tokenId*)$1); }
    |   TK_IDENT '(' parameters_instance ')'
                             { $$ = new AST_function_call( (tokenId*) $1,
                                                           (AST_parameters_list*) $3
                                                         );
                             }
    |   aritmetic_expression { $$ = $1; }
    |   boolean_expression   { $$ = $1; }
;

aritmetic_expression:
        '-' expression %prec NEG 
            { $$ = new AST_un_op((tokenId*)$1, (AST_expression*)$2); }

    |   expression '+' expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression '-' expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression '*' expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression '/' expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }
;

boolean_expression:
        expression TK_AND expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_OR expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_IMP expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_CONSEQ expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   TK_NOT expression %prec NEG
            { $$ = new AST_un_op((tokenId*)$1, (AST_expression*)$2 ); }

    |   expression TK_EQ expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_UNEQ expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_LESS expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_LESS_EQ expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_GREAT expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }

    |   expression TK_GREAT_EQ expression
            { $$ = new AST_op((AST_expression*)$1, (tokenId*)$2, (AST_expression*)$3 ); }
;

// Subgramatica de atributos actuales de llamada a funcion
parameters_instance:
                                    { $$ = new AST_parameters_list();}
    | parameters_instance_non_empty { $$ = $1 }
;

// Subgramatica de atributos actuales de llamada a funcion
parameters_instance_non_empty:
      expression
        { 
            AST_parameters_list* p = new AST_parameters_list();
            p->add_element((AST_expression*)$1);
            $$ = p;
        }
    | parameters_instance_non_empty ',' expression
        {
            ((AST_parameters_list*)$1)->add_element((AST_expression*)$3);
            $$ = $1;
        }
;
%%

void yyerror (char const *s)
{
    error = true;
}

int main (int argc,char **argv)
{
    p = new AST_program();
    error = false;

    if (argc == 1){
        yyparse();
    } else if (argc == 2) {
        yyin = fopen(argv[1], "r");
        do {
            yyparse();
        } while (!feof(yyin));
    }
    if ( error ){
        fprintf (stderr, "Epic fail!\n");
    } else {
        symbol_table st;
        p->fill_and_check(&st);
        p->print(0);
        
        if ( !error ){
            cout << "Like a boss!\n";
            cout << "-------------------------------------------------------\n";
        } else {
            fprintf (stderr, "Epic fail!\n");
        }
        
        
        //st.fill_with(p);
    }
    
    return 0;
}

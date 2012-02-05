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

%token TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
%token TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ

%left '-' '+'
%left '*' '/'
%left NEG
%left TK_AND TK_OR
%right TK_IMP
%left TK_CONSEQ
%left TK_EQ TK_UNEQ
%nonassoc TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ

%type<nd> input declaration variable_declaration block statement
          actual_parameters non_empty_params arg_list non_empty_arg_list
          block_statement loop_statement loop_block loop_block_statement
          else_statements expression aritmetic_expression boolean_expression
%type<tk> TK_TYPE TK_IDENT TK_FUNCTION TK_NONE TK_IF TK_ELSE TK_INT TK_FLOAT
          TK_BOOLEAN TK_CHAR TK_BREAK TK_CONTINUE TK_RETURN TK_ELIF TK_WHILE
          TK_FOR TK_IN TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
          TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ
          '(' ')' '+' '-' '*' '/'

%%

// Definicion de un programa

input : declaration             {
                                    p->add_declaration((AST_declaration*)$1);
                                }
      | declaration input       {
                                    p->add_declaration((AST_declaration*)$1);
                                }
      ;

// Declaracion de variables y funciones

declaration : variable_declaration                                { $$ = $1}
            
            | TK_FUNCTION TK_TYPE TK_IDENT '(' arg_list ')' block
                    { $$ = new AST_function( (tokenType*)$2,
                                             (tokenId*)$3,
                                             (AST_arg_list*)$5,
                                             (AST_block*)$7
                                           );
                    }
            
            | TK_FUNCTION TK_NONE TK_IDENT '(' arg_list ')' block
                    { $$ = new AST_function( 0,
                                             (tokenId*)$3,
                                             (AST_arg_list*)$5,
                                             (AST_block*)$7
                                           );
                    }
            ;

// Declaracion de variables
variable_declaration : TK_TYPE TK_IDENT ';' 
                          { $$ = new AST_variable_declaration((tokenType*)$1,
                                                              (tokenId*)$2
                                                             );
                          }

                     | TK_TYPE TK_IDENT '=' expression ';'
                         { $$ = new AST_variable_declaration((tokenType*)$1,
                                                             (tokenId*)$2,
                                                             (AST_expression*)$4
                                                            );
                         }
                     ;

// Gramatica de la lista de parametros formales de una funcion

arg_list :                              { $$ = new AST_arg_list(); }
         |  TK_TYPE TK_IDENT            { AST_arg_list* ar = new AST_arg_list();
                                          ar->add_argument( (tokenType*)$1,
                                                            (tokenId*)$2
                                                          );
                                          $$ = ar;
                                        }
         |  TK_TYPE TK_IDENT ',' non_empty_arg_list
                         {
                             ((AST_arg_list*)$4)->add_argument( (tokenType*)$1,
                                                               (tokenId*)$2
                                                              );
                             $$ = $4;
                         }
         ;

non_empty_arg_list : TK_TYPE TK_IDENT
                        { AST_arg_list* ar = new AST_arg_list();
                          ar->add_argument( (tokenType*)$1,
                                            (tokenId*)$2
                                          );
                          $$ = ar;
                        }
                   | TK_TYPE TK_IDENT ',' non_empty_arg_list
                       {
                            ((AST_arg_list*)$4)->add_argument( (tokenType*)$1,
                                                              (tokenId*)$2
                                                             );
                            $$ = $4;
                        }
                   ;

// Definicion de bloques de anidamiento

block : '{' block_statement '}'                 { $$ = $2}
      | '{' '}'                                 { $$ = new AST_block(); }
      ;

block_statement : statement { AST_block* b = new AST_block();
                              if ( $1 != 0 )
                                  b->add_statement( (AST_statement*)$1 );
                              $$ = b;
                            }
                | statement block_statement
                            {
                                if ( $1 != 0 ){
                                    ((AST_block*)$2)->add_statement(
                                                            (AST_statement*)$1);
                                }
                                $$ = $2;
                            }
                ;

// TODO
statement : variable_declaration                           { $$ = $1; }
          | TK_IDENT '=' expression ';'
              { 
                  $$ = new AST_assignment((tokenId*)$1, (AST_expression*)$3);
              }
          | ';'                                            { $$ = 0  }

          | TK_IF '(' expression ')' block else_statements 

                   { $$ = new AST_conditional($1,
                                              (AST_expression*) $3,
                                              (AST_block*) $5,
                                              (AST_conditional*) $6
                                             );
                   }

          | TK_WHILE '(' expression ')'   loop_block
                   { $$ = new AST_loop( (token*)$1,
                                        (AST_expression*)$3,
                                        (AST_block*)$5
                                      );
                   }
          | TK_FOR TK_IDENT TK_IN '(' expression ',' expression ')' loop_block
                  {
                      $$ = new AST_bounded_loop( (token*)$1,
                                                 (tokenId*)$2,
                                                 (token*)$3,
                                                 (AST_expression*)$5,
                                                 (AST_expression*)$7,
                                                 (AST_block*)$9);
                  }
          | block                                          { $$ = $1 }
          | TK_RETURN ';'
                   { 
                       $$ = new AST_return($1, 0);    
                   }
          | TK_RETURN expression ';'
                  {         
                       $$ = new AST_return($1,(AST_expression*)$2);
                  }
          ;

// Loop blocks: includes break and continue to statements

loop_block : '{' loop_block_statement '}'           { $$ = $2 }
           | '{' '}'                                { $$ = new AST_block(); }
           ;

loop_block_statement : loop_statement { AST_block* b = new AST_block();
                                        if ( $1 != 0 )
                                            b->add_statement(
                                                            (AST_statement*)$1);
                                        $$ = b;
                                      }
                     | loop_statement loop_block_statement
                                      {
                                        if ( $1 != 0 ){
                                            ((AST_block*)$2)->add_statement(
                                                            (AST_statement*)$1);
                                        }
                                        $$ = $2;
                                       }
                     ;

loop_statement : statement                      { $$ = $1                    }
               | TK_BREAK ';'                   { $$ = new AST_break($1);    }
               | TK_CONTINUE ';'                { $$ = new AST_continue($1); }
               ;

else_statements :                                                  { $$ = 0 }
                | TK_ELIF '(' expression ')' block else_statements
                        { $$ = new AST_conditional($1,
                                                   (AST_expression*) $3,
                                                   (AST_block*) $5,
                                                   (AST_conditional*) $6
                                                   );
                        }
                | TK_ELSE block
                        { $$ = new AST_conditional($1,
                                                   0,
                                                   (AST_block*) $2,
                                                   0
                                                   );
                        }
                ;

// Subgramatica de expresiones
expression : TK_INT                {
                                       $$ = new AST_int( (tokenInt*)$1);
                                   }
           | TK_FLOAT              {
                                       $$ = new AST_float( (tokenFloat*)$1);
                                   }
           | TK_BOOLEAN            {
                                       $$ = new AST_boolean( (tokenBoolean*)$1);
                                   }
           | TK_CHAR               {
                                       $$ = new AST_char( (tokenId*)$1);
                                   }
           | '(' expression ')' {
                                   $$ = new AST_parenthesis( $1,
                                                            (AST_expression*)$2,
                                                             $3);
                                }
           | TK_IDENT                  {
                                           $$ = new AST_ident((tokenId*)$1);
                                       }
           | TK_IDENT '(' actual_parameters ')'
                   {
                       $$ = new AST_function_call( (tokenId*) $1,
                                                   (AST_parameters_list*) $3
                                                 );
                   }
           | aritmetic_expression {
                                     $$ = $1;
                                  }
           | boolean_expression   {
                                     $$ = $1;
                                  }
           ;

aritmetic_expression: '-' expression %prec NEG  {
                                          $$ = new AST_un_op((tokenId*)$1,
                                                             (AST_expression*)$2
                                                            );
                                        }
                    | expression '+' expression
                                       {
                                          $$ = new AST_op((AST_expression*)$1,
                                                           (tokenId*)$2,
                                                          (AST_expression*)$3 );
                                       }
                   | expression '-' expression
                                       {
                                          $$ = new AST_op((AST_expression*)$1,
                                                           (tokenId*)$2,
                                                          (AST_expression*)$3 );
                                       }
                   | expression '*' expression
                                       {
                                          $$ = new AST_op((AST_expression*)$1,
                                                           (tokenId*)$2,
                                                          (AST_expression*)$3 );
                                       }
                   | expression '/' expression
                                       {
                                          $$ = new AST_op((AST_expression*)$1,
                                                           (tokenId*)$2,
                                                          (AST_expression*)$3 );
                                       }
                   ;

boolean_expression: expression TK_AND expression
                        {
                             $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_OR expression
                        {
                             $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_IMP expression
                        {
                             $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_CONSEQ expression
                        {
                             $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | TK_NOT expression %prec NEG
                        {
                             $$ = new AST_un_op((tokenId*)$1,
                                             (AST_expression*)$2 );
                        }
                  | expression TK_EQ expression
                        {
                             $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_UNEQ expression
                        {
                             $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_LESS expression
                        {
                            $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_LESS_EQ expression
                        {
                            $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_GREAT expression
                  
                        {
                            $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  | expression TK_GREAT_EQ expression
                        {
                            $$ = new AST_op((AST_expression*)$1,
                                             (tokenId*)$2,
                                             (AST_expression*)$3 );
                        }
                  ;

// Subgramatica de atributos actuales de llamada a funcion

actual_parameters :                   { $$ = new AST_parameters_list();}
                  | expression        {
                                        AST_parameters_list* p = 
                                            new AST_parameters_list();
                                        p->add_element((AST_expression*)$1);
                                        $$ = p;
                                      }
                  | expression ',' non_empty_params
                                      {
                                        ((AST_parameters_list*)$3)->add_element(
                                             (AST_expression*)$1);
                                          $$ = $3;
                                      }
                  ;

non_empty_params : expression         { AST_parameters_list* p = 
                                                    new AST_parameters_list();
                                        p->add_element((AST_expression*)$1);
                                        $$ = p;
                                      }
                 | expression ',' non_empty_params
                                      {
                                        ((AST_parameters_list*)$3)->add_element(
                                             (AST_expression*)$1);
                                          $$ = $3;
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
/*
    symbol_int* i = new symbol_int("entero", 1);
    symbol_double* d = new symbol_double("doble", 1.0);
    symbol_char* c = new symbol_char("caracter", 'c');
    symbol_table* t = new symbol_table();
    t->insert(i);
    t->insert(d);
    t->insert(c);
    */

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
        p->print(0);
        
        cout << "Like a boss!\n";
        cout << "-------------------------------------------------------\n";
        
        symbol_table st;
        st.fill_with(p);
        
        if ( error ){
            fprintf (stderr, "Epic fail!\n");
        } else {
            cout << "Like a boss!\n";
            cout << "-------------------------------------------------------\n";
        }
    }
    
    return 0;
}

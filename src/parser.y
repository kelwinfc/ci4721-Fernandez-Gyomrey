%{

#include <math.h>
#include <stdio.h>
#include "lib/AST.h"
#include "lib/symbol.h"
#include "lib/symbol_table.h"
#include "lib/llog.h"
#include "lib/type_table.h"

int yylex (void);
void yyerror (char const *);

extern FILE *yyin;

AST_program* p;
llog* logger;
type_table types;

%}

%union{
    AST_node* nd;
    token* tk;
    symbol_table* tt;
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
%token TK_MOD
%token TK_READ TK_PRINT
%token TK_ALIAS TK_NEW_TYPE

%token TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
%token TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ

/* 
 * La razón de esta precedencia es respetar la la precedencia natural de
 * expresiones como true && false == false, considerando la igualdad como
 * equivalencia dicha expresion debe evaluar (true && false) == false, a
 * diferencia de lenguajes como C que evalua true && (false == false).
 */
%left TK_EQ TK_UNEQ
%left TK_CONSEQ
%right TK_IMP
%left TK_AND TK_OR
%nonassoc TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ
%left '-' '+'
%left '*' '/' TK_MOD
%left NEG TK_NOT

%type<nd> input declaration variable_declaration block statement
          parameters_instance parameters_instance_non_empty
          arg_list non_empty_arg_list lista_ident
          block_statement loop_statement loop_block loop_block_statement
          else_statements expression aritmetic_expression boolean_expression
%type<tk> TK_TYPE TK_IDENT TK_FUNCTION TK_NONE TK_IF TK_ELSE
          TK_CONST TK_INT TK_FLOAT TK_BOOLEAN TK_CHAR
          TK_BREAK TK_CONTINUE TK_RETURN TK_ELIF TK_WHILE
          TK_FOR TK_IN TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
          TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ TK_READ TK_PRINT
          '(' ')' '+' '-' '*' '/' TK_MOD '=' ';' ',' '{' '}'
          TK_ALIAS TK_NEW_TYPE

%type<tt> struct_fields;

%%

// Definicion de un programa
input:
        declaration       { if ( $1 )
                                p->add_declaration((AST_declaration*)$1);
                          }
    |   input declaration { if ( $2 )
                                p->add_declaration((AST_declaration*)$2);
                          }
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
            delete $1;
            delete $4;
            delete $6;
            }
    |   TK_FUNCTION TK_NONE TK_IDENT '(' arg_list ')' block
            { $$ = new AST_function( 0,
                                     (tokenId*)$3,
                                     (AST_arg_list*)$5,
                                     (AST_block*)$7
                                   );
            delete $1;
            delete $2;
            delete $4;
            delete $6;
            }
    |   TK_ALIAS TK_IDENT TK_IDENT ';'
            {
                if ( types.has_type( (char*)((tokenId*)$2)->ident.c_str() ) ){
                    types.add_alias( ((tokenId*)$2)->ident,
                                     ((tokenId*)$3)->ident
                                   );
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo '%s' con identificador no definido previamente.",
                             (char*)((tokenId*)$2)->ident.c_str());
                    logger->error($3->line, $3->column, e);

                }
                delete $1;
                delete $2;
                delete $3;
                
                $$ = 0;
            }
    |   TK_NEW_TYPE TK_IDENT '{' struct_fields '}'
            {
                if ( types.has_type( (char*)((tokenId*)$2)->ident.c_str() ) ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo '%s' con identificador repetido.",
                             (char*)((tokenId*)$2)->ident.c_str());
                    logger->error($1->line, $1->column, e);
                    
                    delete $2;
                    delete $4;
                } else {
                    types.add_type( new struct_type( ((tokenId*)$2)->ident, 
                                                     $4)
                                  );
                }
                delete $1;
                delete $3;
                delete $5;
                             
                $$ = 0;
            }
;

struct_fields :     {
                        $$ = new symbol_table();
                    }
              | struct_fields TK_IDENT TK_IDENT ';'
                    {
                        if ( !types.has_type( ((tokenId*)$2)->ident ) )
                        {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Tipo '%s' no definido previamente.",
                                     (char*)((tokenId*)$2)->ident.c_str());
                            logger->error($2->line, $2->column, e);
                            
                            delete $2;
                            delete $4;
                            
                            $1->insert(new symbol( ((tokenId*)$3)->ident, false,
                                               UNDEFINED,
                                               $2->line, $2->column,
                                               false) );
                        } else if ( $1->lookup(((tokenId*)$3)->ident) ){
                            
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Campo '%s' en el tipo '%s' duplicado.",
                                     (char*)((tokenId*)$3)->ident.c_str(),
                                     ((tokenId*)($-1.tk))->ident.c_str()
                                    );
                            logger->error($2->line, $2->column, e);
                        } else {
                            $1->insert(new symbol( ((tokenId*)$3)->ident, false,
                                               types.index_of(
                                                   ((tokenId*)$2)->ident),
                                               $2->line, $2->column,
                                               false) );
                        }
                        
                        $$ = $1;
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
            delete $1;
            delete $4;
            delete $6;
            }
    | TK_TYPE TK_IDENT '=' expression ';'
            { $$ = new AST_variable_declaration( (tokenType*)$1,
                                                 (tokenId*)$2,
                                                 (AST_expression*)$4
                                               );
            delete $3;
            delete $5;
            }
    |   TK_TYPE TK_IDENT ';'
            { $$ = new AST_variable_declaration( (tokenType*)$1,
                                                 (tokenId*)$2,
                                                 0
                                               );
            delete $3;
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
                delete $1;
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
                delete $2;
            }
    |   non_empty_arg_list ',' TK_CONST TK_TYPE TK_IDENT
            {
                ((AST_arg_list*)$1)->add_argument( (tokenType*)$4,
                                                   (tokenId*)$5,
                                                   true
                                                 );
                $$ = $1;
                delete $2;
                delete $3;
            }
;

// Definicion de bloques de anidamiento
block :
        '{' block_statement '}' 
            { 
                $$ = $2;
                delete $1;
                delete $3;
            }
    |   '{' '}'
            { 
                $$ = new AST_block(((tokenId*)$1)->line, ((tokenId*)$1)->column);
                delete $1;
                delete $2;
            }
;

// Bloques de instrucciones
block_statement :
        statement
            {
                AST_block* b = new AST_block(((AST_statement*)$1)->line, ((AST_statement*)$1)->column);
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
            { 
                $$ = new AST_assignment((tokenId*)$1, (AST_expression*)$3);
                delete $2;
                delete $4;
            }
    |   ';'
            { 
                $$ = 0;
                delete $1;
            }
    |   TK_IF '(' expression ')' block else_statements 
            { $$ = new AST_conditional( $1,
                                        (AST_expression*) $3,
                                        (AST_block*) $5,
                                        (AST_conditional*) $6
                                      );
              delete $2;
              delete $4;
            }
    |   TK_WHILE '(' expression ')' loop_block
            { $$ = new AST_loop( (token*)$1,
                                 (AST_expression*)$3,
                                 (AST_block*)$5
                               );
              delete $2;
              delete $4;
            }
    |   TK_FOR TK_IDENT TK_IN '(' expression ',' expression ')' loop_block
            { $$ = new AST_bounded_loop( (token*)$1,
                                         (tokenId*)$2,
                                         (AST_expression*)$5,
                                         (AST_expression*)$7,
                                         (AST_block*)$9);
              delete $3;
              delete $4;
              delete $6;
              delete $8;
            }
    |   block
            { $$ = $1 }
    |   TK_RETURN ';'
            { 
                $$ = new AST_return($1, 0);
                delete $2;
            }
    |   TK_RETURN expression ';'
            {
                $$ = new AST_return($1,(AST_expression*)$2);
                delete $3;
            }
    |   TK_IDENT '(' parameters_instance ')'
                             { $$ = new AST_function_call( (tokenId*) $1,
                                                           (AST_parameters_list*) $3
                                                         );
                               delete $2;
                               delete $4;
                             }
    | TK_READ TK_IDENT ';'  {
                                $$ = new AST_read( (tokenId*)$2 );
                                delete $1;
                                delete $3;
                            }
    | TK_PRINT lista_ident ';'  {
                                    $$ = $2;
                                    delete $1;
                                    delete $3;
                                }
;

lista_ident : expression                 {
                                            $$ = new AST_print();
                                            ((AST_print*)$$)->add_argument( (AST_expression*)$1 );
                                         }
            | lista_ident ',' expression {
                                            ((AST_print*)$1)->add_argument( (AST_expression*)$3 );
                                            $$ = $1;
                                            delete $2;
                                         }
            ;

// Loop blocks: includes break and continue to statements
loop_block:
        '{' loop_block_statement '}'
            { 
                $$ = $2;
                delete $1;
                delete $3;
            }
    |   '{' '}'
            {
                $$ = new AST_block(((tokenId*)$1)->line, ((tokenId*)$1)->column);
                delete $1;
                delete $2;
            }
;

loop_block_statement:
        loop_statement
            {
                AST_block* b = new AST_block(((AST_statement*)$1)->line,
                                             ((AST_statement*)$1)->column
                                            );
                if ( $1 != 0 )
                    b->add_statement((AST_statement*)$1);
                $$ = b;
            }
    |   loop_block_statement loop_statement
            {
                if ( $2 != 0 )
                    ((AST_block*)$1)->add_statement((AST_statement*)$2);
                $$ = $1;
            }
;

loop_statement :
        statement       { $$ = $1                    }
    |   TK_BREAK ';'    { $$ = new AST_break($1); delete $2; }
    |   TK_CONTINUE ';' { $$ = new AST_continue($1); delete $2; }
;

else_statements :
            { $$ = 0 }
    |   TK_ELIF '(' expression ')' block else_statements
            { $$ = new AST_conditional( $1,
                                        (AST_expression*) $3,
                                        (AST_block*) $5,
                                        (AST_conditional*) $6
                                      );
              delete $2;
              delete $4;
            }
    |   TK_ELSE block
            { $$ = new AST_conditional($1, 0, (AST_block*)$2, 0 ); }
;

// Subgramatica de expresiones
expression :
        TK_INT               { $$ = new AST_int( (tokenInt*)$1); }
    |   TK_FLOAT             { $$ = new AST_float( (tokenFloat*)$1); }
    |   TK_BOOLEAN           { $$ = new AST_boolean( (tokenBoolean*)$1); }
    |   TK_CHAR              { $$ = new AST_char( (tokenId*)$1); }
    |   '(' expression ')'   { $$ = (AST_expression*)$2;
                               delete $1;
                               delete $3;
                             }
    |   TK_IDENT             { 
                                AST_expression* e = new AST_ident((tokenId*)$1);
                                
                                $$ = e;
                                
                                if ( e->is_constant ){
                                    $$ = e->constant_folding();
                                }
                                
                                if ( $$ != e ){
                                    delete e;
                                }
                             }
    |   TK_IDENT '(' parameters_instance ')'
                             { $$ = new AST_function_call( (tokenId*) $1,
                                                           (AST_parameters_list*) $3
                                                         );
                               delete $2;
                               delete $4;
                             }
    |   TK_TYPE '(' expression ')' {
                                        $$ = new AST_conversion( (tokenType*) $1,
                                                                 (AST_expression*) $3
                                                               );
                                        delete $2;
                                        delete $4;
                                   }
    |   aritmetic_expression { 
                                $$ = 
                                      ((AST_expression*)$1)->constant_folding();
                                
                                if ( $$ != $1 ){
                                    delete $1;
                                }
                              }
    |   boolean_expression   { 
                                $$ = $1;
                                
                                $$ = 
                                  ((AST_expression*)$1)->constant_folding();
                                
                                if ( $$ != $1 ){
                                    delete $1;
                                }
                             }
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
    |   expression TK_MOD expression
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
            delete $2;
        }
;
%%

void yyerror (char const *s)
{
    logger->error(0, 0, s);
}

int main (int argc,char **argv)
{
    logger = new llog();
    p = new AST_program();

    if (argc == 1){
        yyparse();
    } else if (argc == 2) {
        yyin = fopen(argv[1], "r");
        do {
            yyparse();
        } while (!feof(yyin));
    }
    
    if ( logger->exists_registered_error() ){
        logger->failure("lexer");
        return 0;
    }
    
    if ( logger->exists_registered_error() ){
        logger->failure("parser");
        return 0;
    }
    
    symbol_table st;
    
    p->fill_and_check(&st);
    p->print(0);
    
    if ( logger->exists_registered_error() ){
        logger->failure("context");
        return 0;
    }
    
    logger->success();
    
    return 0;
}

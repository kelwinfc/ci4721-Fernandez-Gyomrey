%{

#include <math.h>
#include <stdio.h>
#include <set>
#include "lib/AST.h"
#include "lib/symbol.h"
#include "lib/symbol_table.h"
#include "lib/llog.h"
#include "lib/type_table.h"
#include "lib/block.h"
#include "lib/inst.h"
#include "lib/optimizer.h"
#include "lib/string_table.h"

using namespace std;

#define DEBUG(L,V) printf((string(L) + " data   : %s (%d, %d) (%d)\n").c_str(), typeid(*V).name(), (*V).line, (*V).column, (*V).uid); printf((string(L) + " return : %s(%d) (%d)\n").c_str(), (*V).has_return == YES ? "YES" : ((*V).has_return == NO ? "NO" : ((*V).has_return == MAYBE ? "MAYBE" : "")), (*V).has_return, (*V).uid);

int yylex (void);
void yyerror (char const *);

extern FILE *yyin;

AST_program* p;
map<string, int> constants;
llog* logger;
type_table types;
vector<uint> offset;
vector<uint> max_offset;
string_table strings;

int num_loops = 0;

AST_discrete_arg_list* discrete_list; // lista global para los parametros
                                      // formales de una funcion memorizable

%}

%union{
    AST_node* nd;
    token* tk;
    symbol_table* tt;
    set<string>* tv;
    int t;
}

/* Bison declarations.  */
%token TK_INT TK_FLOAT TK_BOOLEAN TK_CHAR TK_STRING
%token TK_IDENT
%token TK_FUNCTION TK_REMEMBER TK_FORGET
%token TK_NONE
%token TK_ENUM
%token TK_ENUM_CONSTANT
%token TK_END
%token TK_END_OF_FILE 0
%token TK_IF TK_ELSE TK_ELIF
%token TK_WHILE TK_FOR TK_IN TK_BREAK TK_CONTINUE TK_RETURN
%token TK_CONST
%token TK_MOD
%token TK_READ TK_PRINT
%token TK_ALIAS TK_NEW_TYPE TK_UNION
%token TK_AS
%token TK_POINTER TK_ADDRESS
%token TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
%token TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ
%token TK_FILL TK_WITH TK_MAP

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
%left TK_AS
%right TK_ADDRESS
%left TK_POINTER
%right '['
%left '.'

%type<nd> input declaration variable_declaration block statement
          parameters_instance parameters_instance_non_empty
          arg_list non_empty_arg_list lista_ident string
          block_statement
          else_statements expression aritmetic_expression boolean_expression
          lvalue discrete_type discrete_arg_list non_empty_discrete_arg_list
          
%type<tk> TK_IDENT TK_FUNCTION TK_NONE TK_IF TK_ELSE TK_ENUM TK_ENUM_CONSTANT
          TK_CONST TK_INT TK_FLOAT TK_BOOLEAN TK_CHAR TK_STRING
          TK_BREAK TK_CONTINUE TK_RETURN TK_ELIF TK_WHILE
          TK_FOR TK_IN TK_AND TK_OR TK_IMP TK_CONSEQ TK_EQ TK_UNEQ TK_NOT
          TK_LESS TK_LESS_EQ TK_GREAT TK_GREAT_EQ TK_READ TK_PRINT
          '(' ')' '+' '-' '*' '/' TK_MOD '=' ';' ',' '{' '}' '[' ']' ':' '.'
          TK_ALIAS TK_NEW_TYPE TK_UNION
          TK_AS TK_POINTER TK_ADDRESS
          TK_FILL TK_WITH TK_MAP

%type<tt> struct_fields

%type<t> type_def

%type<tv> enum_values

%%

// Definicion de un programa
input:
        declaration       { 
                            offset.push_back(0);
                            max_offset.push_back(0);
                            
                            if ( $1 )
                                p->add_declaration((AST_declaration*)$1);
                          }
    |   input declaration { if ( $2 )
                                p->add_declaration((AST_declaration*)$2);
                          }
;

type_def:
        TK_IDENT
          {
              $$ = types.index_of( ((tokenId*)$1)->ident );
              delete $1;
          }
      | '[' expression ']' type_def
          {   
              int up = 0;
              int low = 0;
              if ( typeid(*$2) != typeid(AST_int)
                 )
              {
                  char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "Indice de arreglo debe evaluar una constante entera.");
                    logger->error($2->line, $2->column, e);
              } else {
                  up = ((AST_int*)$2)->value;
              }

              array_descriptor* at;
              at = new array_descriptor( types.types[ $4 ],
                                         $4, up, low );
              
              if ( types.has_type( at->name ) ){
                  $$ = types.index_of( at->name );
              } else {
                  $$ = types.add_type( at );
              }
              
              delete $1;
              delete $3;
          }
      | '[' expression ':' expression ']' type_def 
          {
              int up = 0;
              int low = 0;
              if ( typeid(*$2) != typeid(AST_int)
                   || typeid(*$4) != typeid(AST_int)
                 )
              {
                  char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "Indice de arreglo debe evaluar una constante entera.");
                    logger->error($2->line, $2->column, e);
              } else {
                  low = ((AST_int*)$2)->value;
                  up = ((AST_int*)$4)->value;
              }
              
              if ( low > up ){
                  char e[llog::ERR_LEN];
                  snprintf(e, llog::ERR_LEN,
                           "El rango de indices debe ser creciente.");
                  logger->error($2->line, $2->column, e);
              }
              
              array_descriptor* at;
              at = new array_descriptor( types.types[$6],
                                         $6, up, low );
              
              if ( types.has_type( at->name ) ){
                  $$ = types.index_of( at->name );
              } else {
                  $$ = types.add_type( at );
              }

              delete $1;
              delete $3;
              delete $5;
          }
      | '[' error ']' type_def
          {
              char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "Expresion de indices invalida.");
                    logger->error($1->line, $1->column, e);
              
              array_descriptor* at;
              at = new array_descriptor( types.types[$4],
                                         $4, 0, 0 );
              
              if ( types.has_type( at->name ) ){
                  $$ = types.index_of( at->name );
              } else {
                  $$ = types.add_type( at );
              }
              
              delete $1;
              delete $3;
              
              yyerrok;
          }
      | TK_POINTER type_def
          {
              pointer_descriptor* pd = 
                  new pointer_descriptor($2, types.types[$2]->name );

              if ( !types.has_type( pd->name ) ){
                  types.add_type( pd );
              }

              delete $1;
              $$ = types.index_of( pd->name );
          }
      | error TK_IDENT
          {
              char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "Tipo invalido.");
                    logger->error($2->line, $2->column, e);

              yyclearin;
              $$ = INVALID;

              yyerrok;
          }
    ;

// Declaracion de variables y funciones
declaration:
        variable_declaration                               
            { $$ = $1; }
    |   TK_FUNCTION type_def TK_IDENT '(' arg_list ')' block
            {
              if ( !types.is_base( $2 ) ){
                  char e[llog::ERR_LEN];
                  snprintf(e, llog::ERR_LEN,
                           "Funcion '%s' con tipo de retorno no primitivo.",
                           ((tokenId*)$3)->ident.c_str());
                  logger->error($3->line, $3->column, e);
              }
              $$ = new AST_function( $2,
                                     (tokenId*)$3,
                                     (AST_arg_list*)$5,
                                     (AST_block*)$7
                                   );
            delete $1;
            delete $4;
            delete $6;
            }
    | TK_REMEMBER TK_FUNCTION TK_IDENT '(' discrete_arg_list ')' block
            {
                
            }
    |   type_def TK_IDENT '(' arg_list ')' block
            {
              char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "La declaración de funciones debe ser precedida por 'fun'");
                    logger->error($2->line, $2->column, e);

              if ( !types.is_base( $1 ) ){
                  char e[llog::ERR_LEN];
                  snprintf(e, llog::ERR_LEN,
                           "Funcion '%s' con tipo de retorno no primitivo.",
                           ((tokenId*)$2)->ident.c_str());
                  logger->error($2->line, $2->column, e);
              }
              $$ = new AST_function( $1,
                                     (tokenId*)$2,
                                     (AST_arg_list*)$4,
                                     (AST_block*)$6
                                   );
              delete $3;
              delete $5;
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
    |   TK_NONE TK_IDENT '(' arg_list ')' block
            { 
              char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "La declaración de funciones debe ser precedida por 'fun'");
                    logger->error($2->line, $2->column, e);
              $$ = new AST_function( 0,
                                     (tokenId*)$2,
                                     (AST_arg_list*)$4,
                                     (AST_block*)$6
                                   );
              delete $1;
              delete $3;
              delete $5;
            }
    |   TK_ALIAS type_def TK_IDENT ';'
            {
                if ( types.has_type( ((tokenId*)$3)->ident ) ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo '%s' ya fue definido previamente.",
                             ((tokenId*)$3)->ident.c_str());
                    logger->error($3->line, $3->column, e);
                } else {
                    types.add_alias( types.types[$2]->name,
                                     ((tokenId*)$3)->ident
                                   );
                }

                delete $1;
                delete $3;
                delete $4;
                
                $$ = 0;
            }
    |   TK_ALIAS error ';'
            {
                char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Declaracion de alias invalida.");
                    logger->error($1->line, $1->column, e);

                delete $1;
                delete $3;

                $$ = 0;

                yyerrok;
            }
    |   TK_NEW_TYPE TK_IDENT '{' struct_fields '}'
            {
                if ( types.has_type( ((tokenId*)$2)->ident ) ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo '%s' con identificador repetido.",
                             ((tokenId*)$2)->ident.c_str());
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
    |   TK_UNION TK_IDENT '{' struct_fields '}'
            {
                if ( types.has_type( ((tokenId*)$2)->ident ) ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo '%s' con identificador repetido.",
                             ((tokenId*)$2)->ident.c_str());
                    logger->error($1->line, $1->column, e);

                    delete $2;
                    delete $4;
                } else {
                    types.add_type( new union_type( ((tokenId*)$2)->ident, 
                                                     $4)
                                  );
                }
                delete $1;
                delete $3;
                delete $5;

                $$ = 0;
            }
    | TK_ENUM TK_IDENT '{' error '}'
          {
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Error en declaración de valores de enum");
            logger->error($3->line, $3->column, e);
            delete $1;
            delete $2;
            delete $3;
            delete $5;
            $$ = 0;

            yyerrok;
          }
    | TK_ENUM TK_IDENT '{' enum_values '}'
        {
            if ( types.has_type( ((tokenId*)$2)->ident ) ){
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Tipo '%s' con identificador repetido.",
                         ((tokenId*)$2)->ident.c_str());
                logger->error($1->line, $1->column, e);

                delete $2;
                delete $4;
            } else {
                set<string>::iterator it;
                int nt = types.types.size();
                for (it = $4->begin(); it != $4->end(); ++ it) {                   
                    constants[*it] = nt;
                }
                types.add_type( new enum_type( (tokenId*)$2, $4) );
            }
            delete $1;
            delete $3;
            delete $5;

            $$ = 0;
        }
    | error ';' { $$ = 0; }
;

// Declaracion de variables
variable_declaration :
      TK_CONST type_def TK_IDENT '=' expression ';'
            { $$ = new AST_variable_declaration( $2,
                                                 (tokenId*)$3,
                                                 (AST_expression*)$5,
                                                 true
                                               );
             delete $1;
             delete $4;
             delete $6;
            }
    | TK_CONST type_def TK_IDENT '=' error ';' 
            { 
              char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN,
                       "Inicializacion de variable '%s' invalida.\n",
                       ((tokenId*)$3)->ident.c_str()
                      );
              logger->error($3->line, $3->column, e);
              
              $$ = new AST_variable_declaration( $2, (tokenId*)$3,
                                                 0
                                               );
              
              delete $1;
              delete $4;
              delete $6;
              
              yyerrok;
            }
    | type_def TK_IDENT '=' expression ';'
            { $$ = new AST_variable_declaration( $1,
                                                 (tokenId*)$2,
                                                 (AST_expression*)$4
                                               );
              delete $3;
              delete $5;
            }
    | type_def TK_IDENT error expression ';'
            { char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN,
                      "Falta simbolo = entre identificador e inicializacion.\n",
                       ((tokenId*)$2)->ident.c_str()
                      );
              logger->error($2->line, $2->column, e);
              
              $$ = new AST_variable_declaration( $1,
                                                 (tokenId*)$2,
                                                 (AST_expression*)$4
                                               );
              
              delete $5;

              yyerrok;
            }
    | type_def TK_IDENT '=' error ';' 
            { char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN,
                       "Inicializacion de variable '%s' invalida.\n",
                       ((tokenId*)$2)->ident.c_str()
                      );
              logger->error($2->line, $2->column, e);
              
              $$ = new AST_variable_declaration( $1, (tokenId*)$2,
                                                 0
                                               );

              delete $3;
              delete $5;
              
              yyerrok;
            }
    | type_def TK_IDENT ';'
            { $$ = new AST_variable_declaration( $1,
                                                 (tokenId*)$2,
                                                 0
                                               );
              delete $3;

            }
    | type_def TK_IDENT error ';'
            { char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN,
                       "Declaración de variable debe finalizar con ';'.",
                       ((tokenId*)$2)->ident.c_str()
                      );
              logger->error($2->line, $2->column, e);

              $$ = new AST_variable_declaration( $1,
                                                 (tokenId*)$2,
                                                 0
                                               );
              delete $4;

              yyerrok;
            }
;

// Campos internos de los struct
struct_fields :     {
                        $$ = new symbol_table();
                    }
              | struct_fields type_def TK_IDENT ';'
                    {
                        symbol* s = 0;
                        if ( $2 == UNDEFINED )
                        {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Tipo no definido previamente.");
                            logger->error($3->line, $3->column, e);

                            delete $4;

                            s = new symbol( ((tokenId*)$3)->ident, false,
                                               UNDEFINED,
                                               $3->line, $3->column,
                                               false);
                            $1->insert( s );
                        } else if ( $1->lookup(((tokenId*)$3)->ident) ){
                            
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Campo '%s' en definicion de tipo duplicado.",
                                     ((tokenId*)$3)->ident.c_str()
                                    );
                            logger->error($3->line, $3->column, e);
                        } else {
                            s = new symbol( ((tokenId*)$3)->ident, false,
                                               $2,
                                               $3->line, $3->column,
                                               false);
                            $1->insert(s);
                        }

                        $1->accumulate_offset(types.types[$2]->width,
                                              types.types[$2]->alignment);
                        s->offset = $1->accumulated_offset
                                    - types.types[$2]->width;

                        $$ = $1;
                    }
             | struct_fields error ';'
                    {
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                           "Campo %d en definicion de tipo compuesto invalido.",
                            (int)$1->table.size()+1);

                        logger->error($3->line, $3->column, e);

                        delete $3;
                        $$ = $1;
                        yyerrok;
                    }
;

enum_values:
      TK_IDENT
        {
            $$ = new set<string>();
            $$->insert(((tokenId*)$1)->ident);
            delete $1;
        }
    | TK_ENUM_CONSTANT
        {
            $$ = new set<string>();
            
            tokenConstant* tk = (tokenConstant*)$1;
            enum_type* et = (enum_type*)types.types[tk->type];
            
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                         "Constante '%s' definida "
                         "previamente para el tipo '%s' %d:%d",
                         tk->ident.c_str(),
                         et->name.c_str(),
                         et->line, et->column);
            
            logger->error($1->line, $1->column, e);
            delete $1;
        }
    | enum_values ',' TK_IDENT
        {
            $$ = $1;
            
            if ( $1->find(((tokenId*)$3)->ident) != $1->end() ){
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                             "Constante '%s' definida en el tipo enum actual",
                             ((tokenId*)$3)->ident.c_str());
                
                logger->error($3->line, $3->column, e);
            } else {
                $1->insert(((tokenId*)$3)->ident);
            }
            
            delete $2;
            delete $3;
        }
    | enum_values ',' TK_ENUM_CONSTANT
        {
            $$ = $1;
            
            tokenConstant* tk = (tokenConstant*)$3;
            enum_type* et = (enum_type*)types.types[tk->type];
            
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                         "Constante '%s' definida "
                         "previamente para el tipo '%s' %d:%d",
                         tk->ident.c_str(),
                         et->name.c_str(),
                         et->line, et->column);
            
            logger->error($3->line, $3->column, e);
            
            delete $2;
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
        TK_CONST type_def TK_IDENT
            {
                AST_arg_list* ar = new AST_arg_list();
                ar->add_argument( $2,
                                  (tokenId*)$3,
                                  true
                                );
                $$ = ar;
                delete $1;
            }
    |   type_def TK_IDENT
            {
                AST_arg_list* ar = new AST_arg_list();
                ar->add_argument( $1,
                                  (tokenId*)$2
                                );
                $$ = ar;
            }
    |   non_empty_arg_list ',' type_def TK_IDENT
            {
                ((AST_arg_list*)$1)->add_argument( $3,
                                                   (tokenId*)$4
                                                 );
                $$ = $1;
                delete $2;
            }
    |   non_empty_arg_list ',' TK_CONST type_def TK_IDENT
            {
                ((AST_arg_list*)$1)->add_argument( $4,
                                                   (tokenId*)$5,
                                                   true
                                                 );
                $$ = $1;
                delete $2;
                delete $3;
            }
    |   non_empty_arg_list ',' error
            {
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Error en parametro %d de definicion de funcion.\n",
                           (int)((AST_arg_list*)$1)->args.size()+1
                          );
                logger->error($2->line, $2->column, e);
                
                delete $2;
                $$ = $1;
                yyerrok;
            }
;

discrete_arg_list : /* empty */ 
                        { $$ = new AST_discrete_arg_list(); }
                  | non_empty_discrete_arg_list 
                        { 
                          $$ = discrete_list;
                          discrete_list = 0;
                        }
                  ;

discrete_type:
        TK_CONST TK_IDENT TK_IDENT
            {
                if ( types.is_base( ((tokenId*)$2)->ident ) ){
                    
                    TYPE type = types.index_of( ((tokenId*)$2)->ident );
                    
                    if ( type == CHAR ){
                        discrete_list->add_argument( type, (tokenId*)$3,
                                                     0, 255, true );
                    } else if ( type == BOOLEAN ){
                        discrete_list->add_argument( type, (tokenId*)$3,
                                                     0, 1, true );
                    } else if ( type == INT ){
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                                 "Se deben establecer los limites de tipo"
                                 "int.");
                        logger->error($1->line, $1->column, e);
                        delete $2;
                        delete $3;
                    } else {
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                                 "Tipo en funcion memorizable debe ser "
                                 "primitivo discreto.");
                        logger->error($1->line, $1->column, e);
                        delete $2;
                        delete $3;
                    }
                    
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo en funcion memorizable debe ser "
                             "primitivo discreto.");
                    logger->error($1->line, $1->column, e);
                    delete $2;
                    delete $3;
                    
                }
                
                delete $1;
            }
    |   TK_IDENT TK_IDENT
            {
                
                if ( types.is_base( ((tokenId*)$1)->ident ) ){
                    
                    TYPE type = types.index_of( ((tokenId*)$1)->ident );
                    
                    if ( type == CHAR ){
                        discrete_list->add_argument( type, (tokenId*)$2,
                                                     0, 255, false );
                    } else if ( type == BOOLEAN ){
                        discrete_list->add_argument( type, (tokenId*)$2,
                                                     0, 1, false );
                    } else if ( type == INT ){
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                                 "Se deben establecer los limites de tipo"
                                 "int.");
                        logger->error($1->line, $1->column, e);
                        delete $1;
                        delete $2;
                    } else {
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                                 "Tipo en funcion memorizable debe ser "
                                 "primitivo discreto.");
                        logger->error($1->line, $1->column, e);
                        delete $1;
                        delete $2;
                    }
                    
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo en funcion memorizable debe ser "
                             "primitivo discreto.");
                    logger->error($1->line, $1->column, e);
                    delete $1;
                    delete $2;
                }
            }
    |   TK_CONST TK_IDENT TK_IDENT TK_IN '(' expression ',' expression ')'
            {
                if ( types.is_base( ((tokenId*)$2)->ident ) ){
                    
                    TYPE type = types.index_of( ((tokenId*)$2)->ident );
                    
                    if ( type == CHAR ){
                        int upper, lower;
                        upper = lower = 0;
                        
                        if ( typeid(*$6) == typeid(AST_char) ){
                            //TODO verificar si esta escapado
                            lower = ((AST_char*)$6)->value[1];
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite inferior debe evaluar constante "
                                     "char.");
                            logger->error($6->line, $6->column, e);
                        }
                        if ( typeid(*$8) == typeid(AST_char) ){
                            //TODO verificar si esta escapado
                            upper = ((AST_char*)$8)->value[1];
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite superior debe evaluar constante "
                                     "char.");
                            logger->error($8->line, $8->column, e);
                        }
                        
                        discrete_list->add_argument( type, (tokenId*)$3,
                                                     lower, upper, true );
                        
                    } else if ( type == BOOLEAN ){
                        int upper, lower;
                        upper = lower = 0;
                        
                        if ( typeid(*$6) == typeid(AST_boolean) ){
                            //TODO verificar si esta escapado
                            lower = ((AST_char*)$6)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite inferior debe evaluar constante "
                                     "boolean.");
                            logger->error($6->line, $6->column, e);
                        }
                        if ( typeid(*$8) == typeid(AST_boolean) ){
                            //TODO verificar si esta escapado
                            upper = ((AST_char*)$8)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite superior debe evaluar constante "
                                     "boolean.");
                            logger->error($8->line, $8->column, e);
                        }
                        
                        discrete_list->add_argument( type, (tokenId*)$3,
                                                     lower, upper, true );
                        
                    } else if ( type == INT ){
                        
                        int upper, lower;
                        upper = lower = 0;
                        
                        if ( typeid(*$6) == typeid(AST_int) ){
                            //TODO verificar si esta escapado
                            lower = ((AST_char*)$6)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite inferior debe evaluar constante "
                                     "int.");
                            logger->error($6->line, $6->column, e);
                        }
                        if ( typeid(*$8) == typeid(AST_int) ){
                            //TODO verificar si esta escapado
                            upper = ((AST_char*)$8)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite superior debe evaluar constante "
                                     "int.");
                            logger->error($8->line, $8->column, e);
                        }
                        
                        discrete_list->add_argument( type, (tokenId*)$3,
                                                     lower, upper, true );
                        
                    } else {
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                                 "Tipo en funcion memorizable debe ser "
                                 "primitivo discreto.");
                        logger->error($1->line, $1->column, e);
                        delete $2;
                        delete $3;
                    }
                    
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo en funcion memorizable debe ser "
                             "primitivo discreto.");
                    logger->error($1->line, $1->column, e);
                    delete $2;
                    delete $3;
                    
                }
                
                delete $1;
                delete $4;
                delete $5;
                delete $6;
                delete $7;
                delete $8;
                delete $9;
            }
    |   TK_IDENT TK_IDENT TK_IN '(' expression ',' expression ')'
            {
                if ( types.is_base( ((tokenId*)$1)->ident ) ){
                    
                    TYPE type = types.index_of( ((tokenId*)$1)->ident );
                    
                    if ( type == CHAR ){
                        int upper, lower;
                        upper = lower = 0;
                        
                        if ( typeid(*$5) == typeid(AST_char) ){
                            //TODO verificar si esta escapado
                            lower = ((AST_char*)$5)->value[1];
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite inferior debe evaluar constante "
                                     "char.");
                            logger->error($5->line, $5->column, e);
                        }
                        if ( typeid(*$7) == typeid(AST_char) ){
                            //TODO verificar si esta escapado
                            upper = ((AST_char*)$7)->value[1];
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite superior debe evaluar constante "
                                     "char.");
                            logger->error($7->line, $7->column, e);
                        }
                        
                        discrete_list->add_argument( type, (tokenId*)$2,
                                                     lower, upper, true );
                        
                    } else if ( type == BOOLEAN ){
                        int upper, lower;
                        upper = lower = 0;
                        
                        if ( typeid(*$5) == typeid(AST_boolean) ){
                            //TODO verificar si esta escapado
                            lower = ((AST_char*)$5)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite inferior debe evaluar constante "
                                     "boolean.");
                            logger->error($5->line, $5->column, e);
                        }
                        if ( typeid(*$7) == typeid(AST_boolean) ){
                            //TODO verificar si esta escapado
                            upper = ((AST_char*)$7)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite superior debe evaluar constante "
                                     "boolean.");
                            logger->error($7->line, $7->column, e);
                        }
                        
                        discrete_list->add_argument( type, (tokenId*)$2,
                                                     lower, upper, true );
                        
                    } else if ( type == INT ){
                        
                        int upper, lower;
                        upper = lower = 0;
                        
                        if ( typeid(*$5) == typeid(AST_int) ){
                            //TODO verificar si esta escapado
                            lower = ((AST_char*)$5)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite inferior debe evaluar constante "
                                     "int.");
                            logger->error($5->line, $5->column, e);
                        }
                        if ( typeid(*$7) == typeid(AST_int) ){
                            //TODO verificar si esta escapado
                            upper = ((AST_char*)$7)->value[1] ? 1 : 0;
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                     "Limite superior debe evaluar constante "
                                     "int.");
                            logger->error($7->line, $7->column, e);
                        }
                        
                        discrete_list->add_argument( type, (tokenId*)$2,
                                                     lower, upper, true );
                        
                    } else {
                        char e[llog::ERR_LEN];
                        snprintf(e, llog::ERR_LEN,
                                 "Tipo en funcion memorizable debe ser "
                                 "primitivo discreto.");
                        logger->error($1->line, $1->column, e);
                        delete $1;
                        delete $2;
                    }
                    
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Tipo en funcion memorizable debe ser "
                             "primitivo discreto.");
                    logger->error($1->line, $1->column, e);
                    delete $1;
                    delete $2;
                    
                }
                
                delete $3;
                delete $4;
                delete $5;
                delete $6;
                delete $7;
                delete $8;
            }
    ;

// Lista de parámetros que no puede ser vacía
non_empty_discrete_arg_list :
            
            { discrete_list = new AST_discrete_arg_list(); }
            
        discrete_type
            
    |   non_empty_discrete_arg_list ',' discrete_type { delete $2; }
    |   non_empty_discrete_arg_list ',' error         { delete $2; }
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
                AST_block* b = new AST_block(((AST_statement*)$1)->line,
                                             ((AST_statement*)$1)->column);
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
    |   lvalue '=' expression ';'
            { 
                $$ = new AST_assignment((AST_lval*)$1, (AST_expression*)$3);
                delete $2;
                delete $4;
            }
    |   lvalue '=' error ';'
            { 
                $$ = new AST_assignment((AST_lval*)$1, 0);
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Expresión inválida en la asignación.");
                logger->error($2->line, $2->column, e);
                delete $2;
                delete $4;

                yyerrok;
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
    |   TK_IF '(' error ')' block else_statements 
            {   char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Expresión inválida en la expresión de condición.");
                logger->error($2->line, $2->column, e);
                $$ = new AST_conditional( $1,
                                        (AST_expression*) 0,
                                        (AST_block*) $5,
                                        (AST_conditional*) $6
                                      );
              delete $2;
              delete $4;

              yyerrok;
            }
    |   TK_WHILE '(' expression ')' { num_loops++; } block
            { $$ = new AST_loop( (token*)$1,
                                 (AST_expression*)$3,
                                 (AST_block*)$6
                               );
              num_loops--;
              delete $2;
              delete $4;
            }
    |   TK_WHILE '(' error ')' {num_loops++; } block
            { char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN,
                       "Expresión inválida en la expresión de condición.");
              logger->error($2->line, $2->column, e);
              $$ = new AST_loop( (token*)$1,
                                 (AST_expression*)0,
                                 (AST_block*)$6
                               );
              delete $2;
              delete $4;
              num_loops--;
              yyerrok;
            }
    |   TK_FOR TK_IDENT TK_IN '(' expression ',' expression ')' 
        { num_loops++; } block
            { $$ = new AST_bounded_loop( (token*)$1,
                                         (tokenId*)$2,
                                         (AST_expression*)$5,
                                         (AST_expression*)$7,
                                         (AST_block*)$10);
              delete $3;
              delete $4;
              delete $6;
              delete $8;
              num_loops--;
            }
    |   TK_FOR TK_IDENT TK_IN '(' error ')' { num_loops++; } block
            { 
              $$ = new AST_assignment((AST_lval*)$1, 0);
              char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN,
                       "Expresión inválida en límites del for.");
              logger->error($4->line, $4->column, e);
              $$ = new AST_bounded_loop( (token*)$1,
                                         (tokenId*)$2,
                                         (AST_expression*)0,
                                         (AST_expression*)0,
                                         (AST_block*)$8);
              num_loops--;
              delete $3;
              delete $4;
              delete $6;

              yyerrok;
            }
    |   block
            { $$ = $1; }
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
    |   TK_RETURN error ';'
            { 
              $$ = new AST_assignment((AST_lval*)$1, 0);
              char e[llog::ERR_LEN];
              snprintf(e, llog::ERR_LEN, "Return esperando ';'");
              logger->error($1->line, $1->column, e);
              $$ = new AST_return($1, 0);
              delete $3;
              yyerrok;
            }
    |   TK_IDENT '(' parameters_instance ')'
                             { $$ = new AST_procedure_call( (tokenId*) $1,
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
    | TK_READ TK_IDENT error ';'  {
                                char e[llog::ERR_LEN];
                                snprintf(e, llog::ERR_LEN,
                                         "Lista de lectura debe ser terminada ';'");
                                logger->error($1->line, $1->column, e);
                                $$ = 0;
                                delete $1;
                                delete $4;
                                yyerrok;
                            }
    | TK_PRINT lista_ident ';'  {
                                    $$ = $2;
                                    delete $1;
                                    delete $3;
                                }
    | TK_PRINT lista_ident error ';'  {
            
                                    char e[llog::ERR_LEN];
                                    snprintf(e, llog::ERR_LEN,
                                             "Lista de impresión debe ser terminada ';'");
                                    logger->error($1->line, $1->column, e);
            
                                    $$ = 0;
                                    delete $1;
                                    delete $4;
                                    yyerrok;
                                }
    | TK_FILL expression TK_WITH TK_IDENT ';'
        {
            $$ = new AST_fill( (AST_expression*)$2, (tokenId*)$4);
            delete $1;
            delete $3;
            delete $5;
        }
    | TK_MAP TK_IDENT expression ';'
        {
            $$ = new AST_map( (AST_expression*)$3,
                              (AST_expression*)$3,
                              (tokenId*)$2
                             );
            delete $1;
            delete $4;
        }
    | TK_MAP TK_IDENT expression ',' expression ';'
        {
            $$ = new AST_map( (AST_expression*)$3,
                              (AST_expression*)$5,
                              (tokenId*)$2
                             );
            delete $1;
            delete $4;
            delete $6;
        }
    |   TK_BREAK ';'
        { 
            delete $2;
            if ( num_loops > 0 ){
                $$ = new AST_break($1);
            } else {
                
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Sentencia break fuera del alcance de un ciclo.");
                logger->error($1->line, $1->column, e);
                
                delete $1;
                $$ = 0;
            }
        }
    |   TK_CONTINUE ';'
        {
            delete $2;
            if ( num_loops > 0 ){
                $$ = new AST_continue($1);
            } else {
                
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Sentencia continue fuera del alcance de un ciclo.");
                logger->error($1->line, $1->column, e);
                
                delete $1;
                $$ = 0;
            }
        }
    | error ';'
        {
            $$ = 0;
            
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                     "Error en instruccion.");
            logger->error($2->line, $2->column, e);
            
            delete $2;
            yyerrok;
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
            | lista_ident ',' error      {
                                            $$ = $1;
                                            delete $2;
                                         }
            ;

else_statements :
            { $$ = 0; }
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
    |   string               { $$ = $1; }
    |   TK_ENUM_CONSTANT     { $$ = new AST_enum_constant( (tokenConstant*)$1); }
    |   '(' expression ')'   { $$ = (AST_expression*)$2;
                               delete $1;
                               delete $3;
                             }
    |   '(' error ')'        { $$ = new AST_expression();
                               delete $1;
                               delete $3;
                               yyerrok;
                             }
    | lvalue                 {
                                $$ = ((AST_lval*)$1)->constant_folding();
                                if ( $$ != $1 ){
                                    delete $1;
                                }
                             }
    |   TK_IDENT '(' parameters_instance ')'
                             { $$ = new AST_function_call( (tokenId*) $1,
                                                           (AST_parameters_list*) $3
                                                         );
                               delete $2;
                               delete $4;
                             }
    |    expression TK_AS type_def 
        {
            if ( TK_AS == 0 ){
                char e[llog::ERR_LEN];
                  snprintf(e, llog::ERR_LEN,
                           "Conversión a tipo no definido previamente.\n");
                  logger->error($2->line, $2->column, e);
            } else if ( !types.is_base( $3 ) ){
                char e[llog::ERR_LEN];
                  snprintf(e, llog::ERR_LEN,
                           "Conversión a tipo no primitivo '%s'.\n",
                           types.types[$3]->name.c_str());
                  logger->error($2->line, $2->column, e);
            }
            $$ = new AST_conversion( $3,
                                     (AST_expression*) $1
                                   );
            delete $2;
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

string:
        TK_STRING        { $$ = new AST_string( (tokenId*)$1); }    
    |   string TK_STRING { $$ = $1;
                           ((AST_string*)$1)->append((tokenId*)$2);
                         }
;

lvalue :
        TK_IDENT
            { 
                AST_expression* e = new AST_ident((tokenId*)$1);
                
                $$ = e;
                
                if ( e->is_constant ){
                    $$ = e->constant_folding();
                }
                
                if ( $$ != e ){
                    delete e;
                }
             }
     | lvalue '[' expression ']'
             {
                 $$ = new AST_array_access( (AST_lval*)$1, (AST_expression*)$3);
                 delete $2;
                 delete $4;
             }
     | lvalue '[' error ']'
          {
              char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                        "Expresión de índices invalida.");
                    logger->error($2->line, $2->column, e);

              $$ = new AST_array_access( (AST_lval*)$1, 0);
              delete $2;
              delete $4;

              yyerrok;
          }
     | lvalue '.' TK_IDENT
             {
                 $$ = new AST_struct_access( (AST_lval*)$1, (tokenId*)$3);
                 delete $2;
             }
     | lvalue TK_POINTER
             {
                 AST_dereference* dref = new AST_dereference( (AST_lval*)$1 );
                 
                 $$ = dref->constant_folding();
                 
                 if ( $$ != dref ){
                     delete dref;
                 }
                 
                 delete $2;
             }
     | TK_ADDRESS lvalue
             {
                 AST_address* adr = new AST_address( (AST_lval*)$2 );
                 
                 $$ = adr->constant_folding();
                 
                 if ( $$ != adr ){
                     delete adr;
                 }
                 
                 delete $1;
             }
     ;

// Subgramatica de atributos actuales de llamada a funcion
parameters_instance:
                                    { $$ = new AST_parameters_list();}
    | parameters_instance_non_empty { $$ = $1; }
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
    | parameters_instance_non_empty ',' error
        { 
          ((AST_parameters_list*)$1)->add_element((AST_expression*)0);
          $$ = $1;

          char e[llog::ERR_LEN];
          snprintf(e, llog::ERR_LEN, "Expresión de parámetro inválida");
          logger->error($2->line, $2->column, e);

          delete $2;
          yyerrok;
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
    block *b = new block(true);

    if (argc == 1){
        yyparse();
    } else if (argc == 2) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            printf("Archivo '%s' no existe\n", argv[1]);
            exit(1);
        }
        do {
            yyparse();
        } while (!feof(yyin));
    }
    
    symbol_table* st = new symbol_table();
    
    p->fill_and_check(st);
    
    delete st;
    
    if ( logger->exists_registered_error() ){
        logger->dump();
    }
    
    types.dump();
    strings.dump();
    
    p->print(0);

    fprintf(stdout, "-------------------------------------------------------\n\n");
    
    if ( !logger->exists_registered_error() ){
        p->gen_tac(b);
        //useless_jumps(*b);
        b->dump();
    }
    
    fprintf(stdout, "-------------------------------------------------------\n\n");
    
    b->gen_graph();
    delete_unreachable_code();
    
    cout << "Escribiendo bloques básico en bla.blocks"
         << " y grafo de flujo en bla.png\n\n";
    dump_in_file("bla");
    
    fprintf(stdout, "-------------------------------------------------------\n\n");
    
    if ( logger->exists_registered_error() ){
        logger->failure("context");
        return 0;
    }

    logger->success();

    return 0;
}

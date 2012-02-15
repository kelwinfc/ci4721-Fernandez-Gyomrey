#ifndef _AST
#define _AST

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "token.h"
#include "symbol.h"
#include "symbol_table.h"
#include "llog.h"
#include "utils.h"

using namespace std;

extern llog* logger;

/* Clase raiz de la jerarquia del AST */
class AST_node{
    public:

        int line;

        int column;

        virtual void print(int indentation = 0);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_statement : public AST_node{
    public:

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_expression : public AST_node{
    public:
        
        symbol::TYPE type;
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual bool has_functions();
};

class AST_op : public AST_expression {
    public:
        
        enum TYPE { PLUS, MINUS, PROD, DIV, MOD, AND, OR, IMP, CONSEQ,
                    EQ, UNEQ, LESS, LESS_EQ, GREAT, GREAT_EQ     };
        
        int line_op, column_op, line_rop, column_rop;
        
        AST_expression *left, *right;
        
        TYPE oper_type;
        
        AST_op(AST_expression* l, tokenId* o, AST_expression* r);
        
        string binary_operator();
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual bool has_functions();
};

class AST_un_op : public AST_expression {

    public:

        enum TYPE { NEG, NOT };

        TYPE oper_type;

        AST_expression *expr;

        AST_un_op(tokenId* o, AST_expression* e);
        
        string unary_operator();
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual bool has_functions();
};

class AST_int : public AST_expression{
    
    public:

        int value;

        AST_int(tokenInt* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_float : public AST_expression{

    public:

        float value;

        AST_float(tokenFloat* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_boolean : public AST_expression{

    public:

        bool value;

        AST_boolean(tokenBoolean* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_char : public AST_expression {

    public:

        string value;

        AST_char(tokenId* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_ident : public AST_expression {

    public:

        string value;

        symbol* sym;

        AST_ident(tokenId* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_parameters_list : public AST_node {

    public:

        vector< AST_expression* > elem;

        AST_parameters_list();

        void add_element(AST_expression* e);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_function_call : public AST_expression {

    public:

        string name;

        symbol* sym;

        AST_parameters_list* params;

        AST_function_call(tokenId* tk, AST_parameters_list* p);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual bool has_functions();
};

class AST_declaration : public AST_statement {

    public:

        string identifier;
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_variable_declaration : public AST_declaration {
    public:

        symbol* sym;

        AST_expression* value;

        AST_variable_declaration(tokenType* t, tokenId* id,
                                 AST_expression* v = 0,
                                 bool constant = false
                                );

        symbol getSym();
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_block : public AST_node {

    public:

        vector<AST_statement*> statements;

        AST_block(int l, int c);

        void add_statement(AST_statement* s);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_arg_list : public AST_node {
    public:

        vector< symbol* > args;

        AST_arg_list();

        void add_argument( tokenType* t, tokenId* id, bool constant = false );

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_function : public AST_declaration {
    public:

        symbol_function* func;

        AST_arg_list* formal_parameters;

        AST_block* instructions;

        AST_function(tokenType* t, tokenId* id, AST_arg_list* args,
                     AST_block* code);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_program : public AST_node {

    public:

        vector<AST_declaration*> declarations;

        AST_program();

        void add_declaration(AST_declaration* d);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_assignment : public AST_statement {

    public:

        string variable;

        symbol* sym;

        AST_expression* expr;

        AST_assignment(tokenId* i, AST_expression* e);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_return : public AST_statement {

    public:

        AST_expression* expr;

        AST_return(token* tk, AST_expression* e);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_conditional : public AST_statement {

    public:

        AST_expression* expr;

        AST_block* block;

        AST_conditional* else_if;

        AST_conditional(token* tk, AST_expression* e, AST_block* b,
                        AST_conditional* branches);

        virtual void print(int indentation);

        virtual void print(int indentation, bool first);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_loop : public AST_statement {

    public:

        AST_expression* expr;

        AST_block* block;

        AST_loop(token* tk, AST_expression* e, AST_block* b);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};


class AST_bounded_loop : public AST_statement {

    public:

        string name;
        
        symbol* sym;
        
        int line_name, column_name;

        AST_expression* left_bound;

        AST_expression* right_bound;

        AST_block* block;

        AST_bounded_loop(token* for_, tokenId* id,
                         AST_expression* l,
                         AST_expression* r,
                         AST_block* b);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_break : public AST_statement {

    public:

        AST_break(token* t);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_continue: public AST_statement {

    public:

        AST_continue(token* t);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_read : public AST_statement {
    public:
    
        string variable;
        symbol* sym;
        
        AST_read(tokenId* t);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_print : public AST_statement {
    public:
        vector<AST_expression*> list;
        
        AST_print();
        
        void add_argument(AST_expression* e);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
};

class AST_conversion : public AST_expression {
    
    public:
        symbol::TYPE original_type;
        AST_expression* expr;
        
        AST_conversion(tokenType* t, AST_expression* e);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual bool has_functions();
};

#endif
#ifndef _AST
#define _AST

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>
#include "token.h"
#include "symbol.h"
#include "symbol_table.h"
#include "type_table.h"
#include "llog.h"
#include "utils.h"
#include "block.h"

using namespace std;

extern llog* logger;
extern type_table types;

enum RETURN {YES, NO, MAYBE};

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
        
        RETURN has_return;
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_expression : public AST_node{
    public:
        
        TYPE type;
        
        bool is_constant;
        
        // Util en jumping code
        list<int> truelist;
        list<int> falselist;
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval){
            fill_and_check(st);
        }
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual void check_call(symbol_table* st){ fill_and_check(st); }
        
        virtual bool has_functions();
        
        virtual AST_expression* constant_folding();
};

class AST_op : public AST_expression {
    public:
        
        enum OPER_TYPE { PLUS, MINUS, PROD, DIV, MOD, AND, OR, IMP, CONSEQ,
                         EQ, UNEQ, LESS, LESS_EQ, GREAT, GREAT_EQ     };
        
        int line_op, column_op, line_rop, column_rop;
        
        AST_expression *left, *right;
        
        OPER_TYPE oper_type;
        
        AST_op(AST_expression* l, tokenId* o, AST_expression* r);
        
        string binary_operator();
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual bool has_functions();
        
        virtual AST_expression* constant_folding();
};

class AST_un_op : public AST_expression {

    public:
        
        enum OPER_TYPE { NEG, NOT };
        
        OPER_TYPE oper_type;
        
        AST_expression *expr;
        
        AST_un_op(tokenId* o, AST_expression* e);
        
        string unary_operator();
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual bool has_functions();
        
        virtual AST_expression* constant_folding();
};

class AST_int : public AST_expression{
    
    public:

        int value;

        AST_int(tokenInt* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_float : public AST_expression{

    public:

        float value;

        AST_float(tokenFloat* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_boolean : public AST_expression{

    public:

        bool value;
        
        AST_boolean(tokenBoolean* tk);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_char : public AST_expression {

    public:

        string value;

        AST_char(tokenId* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_string : public AST_expression {

    public:

        string value;

        AST_string(tokenId* tk);

        virtual void append(tokenId* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_enum_constant : public AST_expression {

    public:

        string value;

        AST_enum_constant(tokenConstant* tk);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_lval : public AST_expression {
    public:
        
        virtual void print(int indentation);
        
        void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual void fill_and_check(symbol_table* st, bool lval);
        
        virtual AST_expression* constant_folding();
};

class AST_ident : public AST_lval {
    
    public:
        
        string value;

        symbol* sym;

        AST_ident(tokenId* tk);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);
        
        virtual opd *gen_tac(block* b);
        
        virtual void check_call(symbol_table* st);
        
        virtual AST_expression* constant_folding();
};

class AST_dereference : public AST_lval {
    public:
        AST_lval* value;
        
        AST_dereference(AST_lval* l);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_address : public AST_lval {
    public:
        AST_lval* value;
        
        AST_address(AST_lval* l);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_array_access : public AST_lval {
    public:
        AST_lval* value;

        // Puede ser null si existe un error en el parser
        AST_expression* index;
        
        AST_array_access( AST_lval* lvalue, AST_expression* ind);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_struct_access : public AST_lval {
    public:
        AST_lval* value;
        string field;
        
        AST_struct_access( AST_lval* lvalue, tokenId* f);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);
        
        virtual opd *gen_tac(block* b);
        
        virtual AST_expression* constant_folding();
};

class AST_parameters_list : public AST_node {

    public:

        vector< AST_expression* > elem;

        AST_parameters_list();

        void add_element(AST_expression* e);

        virtual void print(int indentation);

        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);

        virtual void fill_and_check(symbol_table* st, vector<bool>& constant);
};

class AST_function_call : public AST_expression {

    public:
        
        RETURN has_return;
        
        string name;

        symbol* sym;

        AST_parameters_list* params;

        AST_function_call(tokenId* tk, AST_parameters_list* p);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual bool has_functions();
        
        virtual AST_expression* constant_folding();
};

class AST_declaration : public AST_statement {

    public:

        string identifier;
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_variable_declaration : public AST_declaration {
    public:

        symbol* sym;

        AST_expression* value;

        AST_variable_declaration(TYPE t, tokenId* id,
                                 AST_expression* v = 0,
                                 bool constant = false
                                );

        symbol getSym();
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_block : public AST_node {

    public:
        
        RETURN has_return;
        
        vector<AST_statement*> statements;

        AST_block(int l, int c);

        void add_statement(AST_statement* s);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_arg_list : public AST_node {
    public:

        vector< symbol* > args;

        AST_arg_list();

        void add_argument( TYPE t, tokenId* id, bool constant = false );

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_discrete_arg_list : public AST_arg_list {
    public:
        vector<pair<int,int> > boundaries;
        
        AST_discrete_arg_list();
        
        void add_argument( TYPE t, tokenId* id, int min_value, int max_value,
                           bool constant = false );
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_function : public AST_declaration {
    public:
        
        symbol_function* func;
        
        AST_arg_list* formal_parameters;
        
        AST_block* instructions;
        
        AST_function(TYPE t, tokenId* id, AST_arg_list* args,
                     AST_block* code);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_program : public AST_node {

    public:

        vector<AST_declaration*> declarations;

        AST_program();

        void add_declaration(AST_declaration* d);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_assignment : public AST_statement {

    public:
        AST_lval* lvalue;
        AST_expression* expr;
        
        AST_assignment(AST_lval* l, AST_expression* e);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_return : public AST_statement {

    public:

        AST_expression* expr;

        AST_return(token* tk, AST_expression* e);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_conditional : public AST_statement {

    public:

        AST_expression* expr;

        AST_block* blck;

        AST_conditional* else_if;

        AST_conditional(token* tk, AST_expression* e, AST_block* b,
                        AST_conditional* branches);

        virtual void print(int indentation);

        virtual void print(int indentation, bool first);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_loop : public AST_statement {

    public:

        AST_expression* expr;

        AST_block* blck;

        AST_loop(token* tk, AST_expression* e, AST_block* b);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};


class AST_bounded_loop : public AST_statement {

    public:

        string name;
        
        symbol* sym;
        
        int line_name, column_name;

        AST_expression* left_bound;

        AST_expression* right_bound;

        AST_block* blck;

        AST_bounded_loop(token* for_, tokenId* id,
                         AST_expression* l,
                         AST_expression* r,
                         AST_block* b);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_break : public AST_statement {

    public:

        AST_break(token* t);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_continue: public AST_statement {

    public:

        AST_continue(token* t);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_read : public AST_statement {
    public:
    
        string variable;
        symbol* sym;
        
        AST_read(tokenId* t);

        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_print : public AST_statement {
    public:
        vector<AST_expression*> list;
        
        AST_print();
        
        void add_argument(AST_expression* e);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_fill : public AST_statement {
    public:
        AST_expression* expr;
        string function;
        symbol* sym;
        
        AST_fill(AST_expression* e, tokenId* f);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_map : public AST_statement {
    public:
        AST_expression* src;
        AST_expression* dst;
        string function;
        symbol* sym;
        
        AST_map(AST_expression* s, AST_expression* d, tokenId* f);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void gen_tac(block* b);
};

class AST_conversion : public AST_expression {
    
    public:
        TYPE original_type;
        AST_expression* expr;
        
        AST_conversion(TYPE t, AST_expression* e);
        
        virtual void print(int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *gen_tac(block* b);
        
        virtual bool has_functions();
};

#endif
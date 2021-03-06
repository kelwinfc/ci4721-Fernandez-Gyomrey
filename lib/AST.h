#ifndef AST_H
#define AST_H

#include <vector>
#include <sstream>
#include <list>
#include <map>
#include <iostream>
#include <iomanip>
#include "llog.h"
#include "token.h"
#include "symbol.h"
#include "symbol_table.h"
#include "type_table.h"
#include "string_table.h"
#include "Tac.h"

using namespace std;

extern llog* logger;
extern type_table types;
extern string_table strings;

enum RETURN {YES, NO, MAYBE};

/* Clase raiz de la jerarquia del AST */
class AST_node{
    public:

        int line;

        int column;

        virtual void dump(ostream &strm, int indentation = 0);
        
        virtual void fill_and_check(symbol_table* st);

        static void dump_indentation(ostream &strm, int indentation);
};

class AST_statement : public AST_node{
    
    public:
        
        RETURN has_return;
        
        list<int> next_list;
        list<int> continue_list;
        list<int> break_list;
        list<int> return_list;
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_expression : public AST_node{
    public:
        
        TYPE type;
        
        bool is_constant;
        
        // Util en jumping code
        list<int> truelist;
        list<int> falselist;
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st, bool lval){
            fill_and_check(st);
        }
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
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
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
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
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual bool has_functions();
        
        virtual AST_expression* constant_folding();
};

class AST_int : public AST_expression{
    
    public:

        int value;

        AST_int(tokenInt* tk);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual AST_expression* constant_folding();
};

class AST_float : public AST_expression{

    public:

        float value;

        AST_float(tokenFloat* tk);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual AST_expression* constant_folding();
};

class AST_boolean : public AST_expression{

    public:

        bool value;
        
        AST_boolean(tokenBoolean* tk);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual AST_expression* constant_folding();
};

class AST_char : public AST_expression {

    public:

        string value;

        AST_char(tokenId* tk);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual AST_expression* constant_folding();
};

class AST_string : public AST_expression {

    public:

        string value;

        int offset;

        AST_string(tokenId* tk);

        virtual void append(tokenId* tk);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual AST_expression* constant_folding();
};

class AST_enum_constant : public AST_expression {
    
    public:
        
        string value;

        int enum_index;
        
        AST_enum_constant(tokenConstant* tk);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual AST_expression* constant_folding();
};

class AST_lval : public AST_expression {
    public:
        
        virtual AST_expression* constant_folding();
        
        void fill_and_check(symbol_table* st);
        
        virtual void fill_and_check(symbol_table* st, bool lval);

        virtual opd *to_tac(Tac* t);

        virtual opd *gen_tac_lval(Tac* t, int *sta_base);

        virtual opd *gen_tac_arr(Tac *t, int *sta_base, opd **ind_addr, int *arr_base);

        static opd *gen_tac_lval_disp(Tac* t, opd *din_base, int *sta_base);
        
        virtual void dump(ostream &strm, int indentation);
};

class AST_ident : public AST_lval {
    
    public:
        
        string value;

        symbol* sym;

        AST_ident(tokenId* tk);
        
        virtual AST_expression* constant_folding();
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);

        virtual void check_call(symbol_table* st);

        virtual opd *gen_tac_lval(Tac* t, int *sta_base);
        
        virtual void dump(ostream &strm, int indentation);
};

class AST_dereference : public AST_lval {
    public:
        AST_lval* value;
        
        AST_dereference(AST_lval* l);
        
        virtual AST_expression* constant_folding();
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);

        virtual opd *gen_tac_lval(Tac* t, int *sta_base);
        
        virtual void dump(ostream &strm, int indentation);
};

class AST_address : public AST_lval {
    public:
        AST_lval* value;

        AST_address(AST_lval* l);

        virtual AST_expression* constant_folding();

        virtual void fill_and_check(symbol_table* st, bool lval = false);

        virtual opd *gen_tac_lval(Tac* t, int *sta_base);

        virtual void dump(ostream &strm, int indentation);
};

class AST_array_access : public AST_lval {
    public:
        AST_lval* value;

        // Puede ser null si existe un error en el parser
        AST_expression* index;
        
        AST_array_access( AST_lval* lvalue, AST_expression* ind);
        
        virtual AST_expression* constant_folding();
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);

        virtual opd *gen_tac_lval(Tac* t, int *sta_base);

        virtual opd *gen_tac_arr(Tac* t, int *sta_base, opd **ind_addr, int *arr_base);
        
        virtual void dump(ostream &strm, int indentation);
};

class AST_struct_access : public AST_lval {
    public:
        AST_lval* value;
        string field;
        symbol* sym;
        bool union_access;
        
        AST_struct_access( AST_lval* lvalue, tokenId* f);
        
        virtual AST_expression* constant_folding();
        
        virtual void fill_and_check(symbol_table* st, bool lval = false);

        virtual opd *gen_tac_lval(Tac* t, int *sta_base);
        
        virtual void dump(ostream &strm, int indentation);
};

class AST_rlval : public AST_lval {

    AST_lval* value;

    public:

        AST_rlval( AST_lval* lvalue);

        virtual AST_expression* constant_folding();

        virtual void fill_and_check(symbol_table* st, bool lval = false);

        virtual opd *to_tac(Tac* t);

        virtual void dump(ostream &strm, int indentation);

};

class AST_parameters_list : public AST_node {

    public:

        vector< AST_expression* > elem;

        AST_parameters_list();

        void add_element(AST_expression* e);

        virtual void dump(ostream &strm, int indentation);

        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);

        virtual void fill_and_check(symbol_table* st, vector<bool>& constant);
};

class AST_function_call : public AST_expression {

    public:

        RETURN has_return;

        string name;

        symbol* sym;

        bool returns_value;

        AST_parameters_list* params;

        AST_function_call(tokenId* tk, AST_parameters_list* p);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual bool has_functions();
        
        virtual AST_expression* constant_folding();
};

class AST_declaration : public AST_statement {

    public:

        string identifier;
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
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
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_block : public AST_statement {

    public:
        RETURN has_return;
        
        vector<AST_statement*> statements;
        
        AST_block(int l, int c);

        void add_statement(AST_statement* s);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_arg_list : public AST_node {
    public:

        vector< symbol* > args;

        AST_arg_list();

        void add_argument( TYPE t, tokenId* id, bool constant = false );

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_discrete_arg_list : public AST_arg_list {
    public:
        vector<pair<int,int> > boundaries;
        
        AST_discrete_arg_list();
        
        void add_argument( TYPE t, tokenId* id, int min_value, int max_value,
                           bool constant = false );
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_function : public AST_declaration {
    public:
        
        symbol_function* func;
        
        AST_arg_list* formal_parameters;
        
        AST_block* instructions;
        
        AST_function(TYPE t, tokenId* id, AST_arg_list* args,
                     AST_block* code);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST : public AST_node {

    public:

        vector<AST_declaration*> declarations;

        AST();

        void add_declaration(AST_declaration* d);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual  Tac* to_tac();
};

class AST_assignment : public AST_statement {

    public:
        AST_lval* lvalue;
        AST_expression* expr;
        
        AST_assignment(AST_lval* l, AST_expression* e);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_procedure_call : public AST_statement {

    public:

        AST_function_call* funcall;

        AST_procedure_call(tokenId* tk, AST_parameters_list* p);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_return : public AST_statement {

    public:

        AST_expression* expr;

        AST_return(token* tk, AST_expression* e);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_conditional : public AST_statement {

    public:

        AST_expression* expr;

        AST_block* blck;

        AST_conditional* else_if;

        AST_conditional(token* tk, AST_expression* e, AST_block* b,
                        AST_conditional* branches);

        virtual void dump(ostream &strm, int indentation);

        virtual void dump(ostream &strm, int indentation, bool first);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_loop : public AST_statement {

    public:

        AST_expression* expr;

        AST_block* blck;

        AST_loop(token* tk, AST_expression* e, AST_block* b);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
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

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_break : public AST_statement {

    public:

        AST_break(token* t);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_continue: public AST_statement {

    public:

        AST_continue(token* t);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_read : public AST_statement {
    public:
    
        string variable;
        symbol* sym;
        
        AST_read(tokenId* t);

        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_print : public AST_statement {
    public:
        vector<AST_expression*> list;
        
        AST_print();
        
        void add_argument(AST_expression* e);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_fill : public AST_statement {
    public:
        AST_expression* expr;
        string function;
        symbol* sym;
        
        AST_fill(AST_expression* e, tokenId* f);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_map : public AST_statement {
    public:
        AST_expression* src;
        AST_expression* dst;
        string function;
        symbol* sym;
        
        AST_map(AST_expression* s, AST_expression* d, tokenId* f);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual void to_tac(Tac* t);
};

class AST_conversion : public AST_expression {
    
    public:
        TYPE original_type;
        AST_expression* expr;
        
        AST_conversion(TYPE t, AST_expression* e);
        
        virtual void dump(ostream &strm, int indentation);
        
        virtual void fill_and_check(symbol_table* st);
        
        virtual opd *to_tac(Tac* t);
        
        virtual bool has_functions();
};

#endif
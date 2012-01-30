#ifndef _AST
#define _AST

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "token.h"

/* Clase raiz de la jerarquia del AST */
class AST_node{
	public:
		
		int line;
		
		AST_node(int l = 0);
		
		void print(int indentation = 0);
};

class AST_statement : public AST_node{
	public:

		AST_statement();

		void print(int indentation);
};

class AST_type : public AST_node {
    private:

		enum TYPE { INT, FLOAT, CHAR, BOOLEAN };

    public:

        TYPE type;
        
        AST_type(int l, char* t);
        
        void print(int indentation);
};

class AST_expression : public AST_node{

    public:

        AST_expression();

        void print(int indentation);
};

class AST_parenthesis : public AST_expression {

    public:

        AST_expression* expr;

        token *left, *right;

        AST_parenthesis(token* l, AST_expression* e, token* r);

        void print(int indentation);
};

class AST_op : public AST_expression {

    public:

        AST_expression *left, *right;

        tokenId *op;

        AST_op(AST_expression* l, tokenId* o, AST_expression* r);

        void print(int indentation);
};

class AST_un_op : public AST_expression {
    
    public:

        AST_expression *expr;

        tokenId *op;

        AST_un_op(tokenId* o, AST_expression* e);

        void print(int indentation);
};

class AST_int : public AST_expression{
    
    public:

        tokenInt* value;

        AST_int(tokenInt* tk);

        void print(int indentation);
};

class AST_float : public AST_expression{

    public:

        tokenFloat* value;

        AST_float(tokenFloat* tk);

        void print(int indentation);
};

class AST_boolean : public AST_expression{

    public:

        tokenBoolean* value;

        AST_boolean(tokenBoolean* tk);

        void print(int indentation);
};

class AST_ident : public AST_expression {

    public:

        tokenId* value;

        AST_ident(tokenId* tk);

        void print(int indentation);
};

class AST_char : public AST_expression {

    public:

        tokenId* value;

        AST_char(tokenId* tk);

        void print(int indentation);
};

class AST_parameters_list : public AST_node {

    public:

        vector< AST_expression* > elem;

        AST_parameters_list();

        void add_element(AST_expression* e);

        void print(int indentation);
};

class AST_function_call : public AST_expression {

    public:

        tokenId* ident;

        AST_parameters_list* params;

        AST_function_call(tokenId* tk, AST_parameters_list* p);

        void print(int indentation);
};

class AST_declaration : public AST_statement {

    public:

        tokenType* type;

        tokenId* identifier;

        AST_declaration();

        AST_declaration( tokenType* t, tokenId* id );

        void print(int indentation);
};

class AST_variable_declaration : public AST_declaration {

    public:

        AST_expression* value;

        AST_variable_declaration(tokenType* t, tokenId* id,
                                 AST_expression* v = 0
                                );
        
        void print(int indentation);
};

class AST_block : public AST_node {

    public:

        vector<AST_statement*> statements;

        AST_block();

        void add_statement(AST_statement* s);

        void print(int indentation);
};

class AST_arg_list : public AST_node {

    public:

        vector< pair<tokenType*, tokenId*> > args;

        AST_arg_list();

        void add_argument( tokenType* t, tokenId* id );

        void print(int indentation);
};

class AST_function : public AST_declaration {

    public:

        AST_arg_list* formal_parameters;

        AST_block* instructions;

        AST_function(tokenType* t, tokenId* id, AST_arg_list* args,
                     AST_block* code);

        void print(int indentation);
};

class AST_program : public AST_node {

    public:

        vector<AST_declaration*> declarations;

        AST_program();

        void add_declaration(AST_declaration* d);

        void print(int indentation);
};

class AST_assignment : public AST_statement {

    public:

        tokenId* variable;

        AST_expression* expr;

        AST_assignment(tokenId* i, AST_expression* e);

        void print(int indentation);
};

class AST_return : public AST_statement {

    public:

        token* ret;

        AST_expression* expr;

        AST_return(token* tk, AST_expression* e);

        void print(int indentation);
};

class AST_conditional : public AST_statement {

    public:

        token* tk_if;

        AST_expression* expr;

        AST_block* block;

        AST_conditional* else_if;

        AST_conditional(token* tk, AST_expression* e, AST_block* b,
                        AST_conditional* branches);

        void print(int indentation);

        void print(int indentation, bool first);
};

class AST_loop : public AST_statement {

    public:

        token* tk_while;

        AST_expression* expr;

        AST_block* block;

        AST_loop(token* tk, AST_expression* e, AST_block* b);

        void print(int indentation);
};


class AST_bounded_loop : public AST_statement {

    public:

        token* tk_for;

        tokenId* tk_ident;

        token* tk_in;

        AST_expression* left_bound;

        AST_expression* right_bound;

        AST_block* block;

        AST_bounded_loop(token* for_, tokenId* id, token* in,
                         AST_expression* l,
                         AST_expression* r,
                         AST_block* b);

        void print(int indentation);
};

class AST_break : public AST_statement {

    public:

        token* tk;

        AST_break(token* t);

        void print(int indentation);
};

class AST_continue: public AST_statement {

    public:

        token* tk;

        AST_continue(token* t);

        void print(int indentation);
};


#endif
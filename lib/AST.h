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
        
        AST_node(){
            line = 0;
        }
        
        AST_node(int l){
            line = l;
        }
        
        virtual void print(int indentation = 0){}
};

class AST_statement: public AST_node {
    public:
        
        AST_statement(){}
        
        virtual void print(int indentation);
};

class AST_type : public AST_node {
    enum TYPE { INT, FLOAT, CHAR, BOOLEAN };
    
    public:
        TYPE type;
        
        AST_type(int l, char* t);
        
        virtual void print(int indentation);
};

class AST_expression : public AST_node{
    
    public:
        
        AST_expression(){}
        
        virtual void print(int indentation);
    
};

class AST_parenthesis : public AST_expression {
    
    public:
        AST_expression* expr;
        token *left, *right;
        
        AST_parenthesis(token* l, AST_expression* e, token* r){
            expr = e;
            left = l;
            right = r;
        }
        
        virtual void print(int indentation);
    
};

class AST_op : public AST_expression {
    
    public:
        AST_expression *left, *right;
        tokenId *op;
        
        AST_op(AST_expression* l, tokenId* o, AST_expression* r){
            op = o;
            left = l;
            right = r;
        }
        
        virtual void print(int indentation);
    
};

class AST_un_op : public AST_expression {
    
    public:
        AST_expression *expr;
        tokenId *op;
        
        AST_un_op(tokenId* o, AST_expression* e){
            op = o;
            expr = e;
        }
        
        virtual void print(int indentation);
    
};

class AST_int : public AST_expression{
    
    public:
        tokenInt* value;
        
        AST_int(tokenInt* tk){ value = tk; }
        
        virtual void print(int indentation);
    
};

class AST_float : public AST_expression{
    
    public:
        tokenFloat* value;
        
        AST_float(tokenFloat* tk){ value = tk; }
        
        virtual void print(int indentation);
};

class AST_boolean : public AST_expression{
    
    public:
        tokenBoolean* value;
        
        AST_boolean(tokenBoolean* tk){ value = tk; }
        
        virtual void print(int indentation);
    
};

class AST_ident : public AST_expression {
    
    public:
        tokenId* value;
        
        AST_ident(tokenId* tk){ value = tk; }
        
        virtual void print(int indentation);
    
};

class AST_char : public AST_expression {
    
    public:
        tokenId* value;
        
        AST_char(tokenId* tk){ value = tk; }
        
        virtual void print(int indentation);
    
};

class AST_parameters_list : public AST_node {
    
    public:
        vector< AST_expression* > elem;
        
        AST_parameters_list(){
            elem.clear();
        }
        
        void add_element(AST_expression* e){
            elem.push_back( e );
        }
        
        virtual void print(int indentation);
};

class AST_function_call : public AST_expression {
    
    public:
        tokenId* ident;
        AST_parameters_list* params;
        
        AST_function_call(tokenId* tk, AST_parameters_list* p){
            ident = tk;
            params = p;
        }
        
        virtual void print(int indentation);
};

class AST_declaration: public AST_statement {
    public:
        tokenType* type;
        tokenId* identifier;
        
        AST_declaration(){
            type = 0;
            identifier = 0;
        }
        
        AST_declaration( tokenType* t, tokenId* id ){
            type = t;
            identifier = id;
        }
        
        virtual void print(int indentation);
};

class AST_variable_declaration: public AST_declaration {
    public:
        AST_expression* value;
        
        AST_variable_declaration(tokenType* t, tokenId* id,
                                 AST_expression* v = 0
                                )
        {
            type = t;
            identifier = id;
            value = v;
        }
        
        virtual void print(int indentation);
};

class AST_block: public AST_node {
    public:
        vector<AST_statement*> statements;
        
        AST_block(){ statements.clear(); }
        
        void add_statement(AST_statement* s){
            statements.push_back(s);
        }
        
        virtual void print(int indentation);
};

class AST_arg_list: public AST_node {
    public:
        vector< pair<tokenType*, tokenId*> > args;
        
        AST_arg_list(){}
        
        void add_argument( tokenType* t, tokenId* id ){
            args.push_back( pair<tokenType*, tokenId*>(t,id) );
        }
        
        virtual void print(int indentation);
};

class AST_function: public AST_declaration {
    public:
        AST_arg_list* formal_parameters;
        AST_block* instructions;
        
        AST_function(tokenType* t, tokenId* id, AST_arg_list* args,
                     AST_block* code)
        {
            type = t;
            identifier = id;
            formal_parameters = args;
            instructions = code;
        }
        
        virtual void print(int indentation);
};

class AST_program: public AST_node {
    public:
        vector<AST_declaration*> declarations;
        
        AST_program(){}
        
        void add_declaration(AST_declaration* d){
            declarations.push_back(d);
        }
        
        virtual void print(int indentation);
};

class AST_assignment : public AST_statement {
    public:
        tokenId* variable;
        AST_expression* expr;
        
        AST_assignment(tokenId* i, AST_expression* e){
            variable = i;
            expr = e;
        }
        
        virtual void print(int indentation);
};

class AST_return : public AST_statement {
    public:
        token* ret;
        AST_expression* expr;
        
        AST_return(token* tk, AST_expression* e){
            ret = tk;
            expr = e;
        }
        
        virtual void print(int indentation);
};

class AST_conditional : public AST_statement {
    public:
        token* tk_if;
        AST_expression* expr;
        AST_block* block;
        AST_conditional* else_if;
        
        AST_conditional(token* tk, AST_expression* e, AST_block* b,
                        AST_conditional* branches)
        {
            tk_if = tk;
            expr = e;
            block = b;
            else_if = branches;
        }
        
        void print(int indentation){
            print(indentation, true);
        }
        
        virtual void print(int indentation, bool first);
};

class AST_loop : public AST_statement {
    public:
        token* tk_while;
        AST_expression* expr;
        AST_block* block;
        
        AST_loop(token* tk, AST_expression* e, AST_block* b)
        {
            tk_while = tk;
            expr = e;
            block = b;
        }
        
        virtual void print(int indentation);
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
                         AST_block* b)
        {
            tk_for = for_;
            tk_ident = id;
            tk_in = in;
            
            left_bound = l;
            right_bound = r;
            
            block = b;
        }
        
        virtual void print(int indentation);
};

class AST_break : public AST_statement {
    public:
        token* tk;
        
        AST_break(token* t)
        {
            tk = t;
        }
        
        virtual void print(int indentation){
            printf("%3d: ", tk->line);
            print_indentation(indentation);
            cout << "Break\n";
        }
};

class AST_continue: public AST_statement {
    public:
        token* tk;
        
        AST_continue(token* t)
        {
            tk = t;
        }
        
        virtual void print(int indentation){
            printf("%3d: ", tk->line);
            print_indentation(indentation);
            cout << "Continue\n";
        }
};

#endif
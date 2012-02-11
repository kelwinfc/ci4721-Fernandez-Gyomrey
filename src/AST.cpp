#include "AST.h"
#include "utils.h"
#include "AST/print.cpp"
#include "AST/check.cpp"

AST_op::AST_op(AST_expression* l, tokenId* o, AST_expression* r){

    type = symbol::UNDEFINED;

    line = l->line;
    column = l->column;

    if ( o->ident.compare("+") == 0 ){
        oper_type = PLUS;
    } else if ( o->ident.compare("-") == 0 ){
        oper_type = MINUS;
    } else if ( o->ident.compare("*") == 0 ){
        oper_type = PROD;
    } else if ( o->ident.compare("/") == 0 ){
        oper_type = DIV;
    } else if ( o->ident.compare("&&") == 0 ){
        oper_type = AND;
    } else if ( o->ident.compare("||") == 0 ){
        oper_type = OR;
    } else if ( o->ident.compare("==>") == 0 ){
        oper_type = IMP;
    } else if ( o->ident.compare("<==") == 0 ){
        oper_type = CONSEQ;
    } else if ( o->ident.compare("==") == 0 ){
        oper_type = EQ;
    } else if ( o->ident.compare("!=") == 0 ){
        oper_type = UNEQ;
    } else if ( o->ident.compare("<") == 0 ){
        oper_type = LESS;
    } else if ( o->ident.compare("<=") == 0 ){
        oper_type = LESS_EQ;
    } else if ( o->ident.compare(">") == 0 ){
        oper_type = GREAT;
    } else if ( o->ident.compare(">=") == 0 ){
        oper_type = GREAT_EQ;
    } else {
        throw "Error de implementación en identificación de operador de expresión binaria";
    }

    line_op = o->line;
    column_op = o->column;
    free(o);

    left = l;
    right = r;
}

AST_un_op::AST_un_op(tokenId* o, AST_expression* e){

    type = symbol::UNDEFINED;
    
    if ( o->ident.compare("!") == 0 ){
        oper_type = NOT;
    } else if ( o->ident.compare("-") == 0 ){
        oper_type = NEG;
    } else {
        throw "Error de implementación en identificación de operador de expresión unaria";
    }

    line = o->line;
    column = o->column;
    free(o);

    expr = e;
}

AST_int::AST_int(tokenInt* tk){
    
    type = symbol::INT;
    
    line = tk->line;
    column = tk->column;
    value = tk->number;
    free(tk);
}

AST_float::AST_float(tokenFloat* tk){

    type = symbol::FLOAT;
    
    line = tk->line;
    column = tk->column;
    value = tk->number;
    free(tk);
}

AST_boolean::AST_boolean(tokenBoolean* tk){
    
    type = symbol::BOOLEAN;
    
    line = tk->line;
    column = tk->column;
    value = tk->value;
    free(tk);
}

AST_char::AST_char(tokenId* tk){

    type = symbol::CHAR;
    
    line = tk->line;
    column = tk->column;
    value = tk->ident + "";
    free(tk);
}

AST_ident::AST_ident(tokenId* tk){

    type = symbol::UNDEFINED;
    
    line = tk->line;
    column = tk->column;
    value = string(tk->ident);
    free(tk);
}

AST_parameters_list::AST_parameters_list(){
    elem.clear();
}

void AST_parameters_list::add_element( AST_expression* e ){
    if ( elem.empty() ) {
        line = e->line;
        column = e->column;
    }

    elem.push_back( e );
}

AST_function_call::AST_function_call(tokenId* tk, AST_parameters_list* p){
    line = tk->line;
    column = tk->column;
    name = string(tk->ident);
    free(tk);

    params = p;
}

AST_variable_declaration::AST_variable_declaration(tokenType* t, tokenId* id,
                                                   AST_expression* v
                                                  )
{
    line = t->line;
    
    sym = new symbol(string(id->ident), false, t->ident, id->line,
                     id->column, 0 != v
                    );
    free(t);
    free(id);

    value = v;
}

AST_block::AST_block(){
    statements.clear();
}

void AST_block::add_statement(AST_statement* s){
    statements.push_back(s);
}

AST_arg_list::AST_arg_list(){

}

void AST_arg_list::add_argument( tokenType* t, tokenId* id ){
    symbol* next_arg = new symbol(id->ident, false, t->ident,
                                  t->line, t->column, false);
    
    args.push_back( next_arg );
    
    line = args[0]->getLine();
}


AST_function::AST_function(tokenType* t, tokenId* id, AST_arg_list* args,
                           AST_block* code
                          )
{
    line = id->line;
    column = id->column;
    
    vector< symbol* >::iterator it;
    vector< symbol::TYPE > types;
    for (it = args->args.begin(); it != args->args.end(); ++ it){
        types.push_back( (*it)->getType() );
    }
    
    if ( t ){
        func = new symbol_function(id->ident, t->ident, id->line, id->column,
                                   types);
    } else {
        func = new symbol_function(id->ident, id->line, id->column, types );
    }
    
    free(t);
    free(id);
    
    formal_parameters = args;
    instructions = code;
}

AST_program::AST_program(){

}

void AST_program::add_declaration(AST_declaration* d){
    declarations.push_back(d);
}

AST_assignment::AST_assignment(tokenId* i, AST_expression* e){
    
    variable = string(i->ident);
    line = i->line;
    column = i->column;
    free(i);

    expr = e;
}

AST_return::AST_return(token* tk, AST_expression* e){

    line = tk->line;
    column = tk->column;
    free(tk);

    expr = e;
}

AST_conditional::AST_conditional(token* tk, AST_expression* e, AST_block* b,
                                 AST_conditional* branches)
{
    line = tk->line;
    column = tk->column;
    free(tk);

    expr = e;
    block = b;
    else_if = branches;
}

AST_loop::AST_loop(token* tk, AST_expression* e, AST_block* b)
{
    line = tk->line;
    column = tk->column;
    free(tk);

    expr = e;
    block = b;
}

AST_bounded_loop::AST_bounded_loop(token* for_, tokenId* id,
                         AST_expression* l,
                         AST_expression* r,
                         AST_block* b)
{
    line = for_->line;
    column = for_->column;
    free(for_);

    name = string(id->ident);
    line_name = id->line;
    column_name = id->column;
    free(id);
    
    left_bound = l;
    right_bound = r;
    
    sym = 0;
    block = b;
}

AST_break::AST_break(token* t){
    line = t->line;
    column = t->column;
    free(t);
}

AST_continue::AST_continue(token* t){
    line = t->line;
    column = t->column;
    free(t);
}


/*
AST_declaration::AST_declaration(tokenType* t, tokenId* i){
    line = t->line;
    
    id = (char*)malloc( strlen(i->ident)*sizeof(char) );
    memset(id, 0, sizeof(i->ident));
    strcpy(id, i->ident);
    
    val = 0;
}

AST_declaration::AST_declaration(tokenType* t, tokenId* i, AST_expression* expr)
{
    AST_declaration(t,i);
    val = expr;
}

*/

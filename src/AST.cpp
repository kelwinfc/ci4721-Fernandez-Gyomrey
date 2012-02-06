#include "AST.h"
#include "utils.h"
#include "AST/print.cpp"
        
AST_node::AST_node(int l){
    line = l;
}

AST_statement::AST_statement(){

}

AST_expression::AST_expression(){

}

AST_op::AST_op(AST_expression* l, tokenId* o, AST_expression* r){
    op = o;
    left = l;
    right = r;
}

AST_un_op::AST_un_op(tokenId* o, AST_expression* e){
    op = o;
    expr = e;
}

AST_int::AST_int(tokenInt* tk){
    value = tk;
}

AST_float::AST_float(tokenFloat* tk){
    value = tk;
}

AST_boolean::AST_boolean(tokenBoolean* tk){
    value = tk;
}

AST_ident::AST_ident(tokenId* tk){
    value = tk;
}

//TODO char recibe un id?
AST_char::AST_char(tokenId* tk){
    value = tk;
}

AST_parameters_list::AST_parameters_list(){
    elem.clear();
}

void AST_parameters_list::add_element(AST_expression* e){
    elem.push_back( e );
}

AST_function_call::AST_function_call(tokenId* tk, AST_parameters_list* p){
    ident = tk;
    params = p;
}

AST_declaration::AST_declaration(){
    type = 0;
    identifier = 0;
}

AST_declaration::AST_declaration( tokenType* t, tokenId* id ){
    type = t;
    identifier = id;
}


AST_variable_declaration::AST_variable_declaration(tokenType* t, tokenId* id,
                                                   AST_expression* v
                                                  )
{
    type = t;
    identifier = id;
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
    args.push_back( pair<tokenType*, tokenId*>(t,id) );
}


AST_function::AST_function(tokenType* t, tokenId* id, AST_arg_list* args,
             AST_block* code)
{
    type = t;
    identifier = id;
    formal_parameters = args;
    instructions = code;
}

AST_program::AST_program(){

}

void AST_program::add_declaration(AST_declaration* d){
    declarations.push_back(d);
}

AST_assignment::AST_assignment(tokenId* i, AST_expression* e){
    variable = i;
    expr = e;
}

AST_return::AST_return(token* tk, AST_expression* e){
    ret = tk;
    expr = e;
}

AST_conditional::AST_conditional(token* tk, AST_expression* e, AST_block* b,
                                 AST_conditional* branches)
{
    tk_if = tk;
    expr = e;
    block = b;
    else_if = branches;
}

AST_loop::AST_loop(token* tk, AST_expression* e, AST_block* b)
{
    tk_while = tk;
    expr = e;
    block = b;
}

AST_bounded_loop::AST_bounded_loop(token* for_, tokenId* id, token* in,
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

AST_break::AST_break(token* t){
    tk = t;
}

AST_continue::AST_continue(token* t){
    tk = t;
}

AST_type::AST_type(int l, char* t){
    line = l;
    
    if ( strcmp(t, "int") == 0 ){
        type = INT;
    } else if ( strcmp(t, "float") == 0 ){
        type = FLOAT;
    } else if ( strcmp(t, "char") == 0 ){
        type = CHAR;
    } else if ( strcmp(t, "boolean") == 0 ){
        type = BOOLEAN;
    } else {
        //TODO qué pasa aquí
    }
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

void AST_declaration::print(int indentation){
    cout << "Declaration: " << id 
         << " (";
     type->print(0);
     cout << ")";
    
    if ( val != 0 ){
        cout << " initialized with:\n";
        val->print(indentation+1);
    } else {
        cout << " uninitialized:\n";
    }
}
*/

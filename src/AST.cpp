#include "AST.h"
#include "utils.h"
#include "AST/print.cpp"

AST_op::AST_op(AST_expression* l, tokenId* o, AST_expression* r){

    line = l->line;
    column = l->column;

    if ( strcmp(o->ident, "+") == 0 ){
        type = PLUS;
    } else if ( strcmp(o->ident, "-") == 0 ){
        type = MINUS;
    } else if ( strcmp(o->ident, "*") == 0 ){
        type = PROD;
    } else if ( strcmp(o->ident, "/") == 0 ){
        type = DIV;
    } else if ( strcmp(o->ident, "&&") == 0 ){
        type = AND;
    } else if ( strcmp(o->ident, "||") == 0 ){
        type = OR;
    } else if ( strcmp(o->ident, "==>") == 0 ){
        type = IMP;
    } else if ( strcmp(o->ident, "<==") == 0 ){
        type = CONSEQ;
    } else if ( strcmp(o->ident, "==") == 0 ){
        type = EQ;
    } else if ( strcmp(o->ident, "!=") == 0 ){
        type = UNEQ;
    } else if ( strcmp(o->ident, "<") == 0 ){
        type = LESS;
    } else if ( strcmp(o->ident, "<=") == 0 ){
        type = LESS_EQ;
    } else if ( strcmp(o->ident, ">") == 0 ){
        type = GREAT;
    } else if ( strcmp(o->ident, ">=") == 0 ){
        type = GREAT_EQ;
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

    if ( strcmp(o->ident, "!") == 0 ){
        type = NOT;
    } else if ( strcmp(o->ident, "-") == 0 ){
        type = NEG;
    } else {
        throw "Error de implementación en identificación de operador de expresión unaria";
    }

    line = o->line;
    column = o->column;
    free(o);

    expr = e;
}

AST_int::AST_int(tokenInt* tk){

    line = tk->line;
    column = tk->column;
    value = tk->number;
    free(tk);
}

AST_float::AST_float(tokenFloat* tk){

    line = tk->line;
    column = tk->column;
    value = tk->number;
    free(tk);
}

AST_boolean::AST_boolean(tokenBoolean* tk){

    line = tk->line;
    column = tk->column;
    value = tk->value;
    free(tk);
}

AST_ident::AST_ident(tokenId* tk){

    line = tk->line;
    column = tk->column;
    value = string(tk->ident);
    free(tk);
}

AST_char::AST_char(tokenId* tk){

    line = tk->line;
    column = tk->column;
    value = *tk->ident;
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
    if ( strcmp(t->type(), "int") == 0 ){
        type = INT;
    } else if ( strcmp(t->type(), "float") == 0 ){
        type = FLOAT;
    } else if ( strcmp(t->type(), "char") == 0 ){
        type = CHAR;
    } else if ( strcmp(t->type(), "boolean") == 0 ){
        type = BOOLEAN;
    } else {
        throw "Error en implementación de identificación de tipo de variable";
    }
    line = t->line;
    column = t->column;
    free(t);

    line_id = id->line;
    column_id = id->column;
    identifier = string(id->ident);
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
    TYPE type;
    if ( strcmp(t->type(), "int") == 0 ){
        type = INT;
    } else if ( strcmp(t->type(), "float") == 0 ){
        type = FLOAT;
    } else if ( strcmp(t->type(), "char") == 0 ){
        type = CHAR;
    } else if ( strcmp(t->type(), "boolean") == 0 ){
        type = BOOLEAN;
    } else {
        throw "Error en implementación de identificación de tipo de parámetro de función";
    }
    args.push_back( pair<TYPE, string>(type, string(id->ident)) );
}


AST_function::AST_function(tokenType* t, tokenId* id, AST_arg_list* args,
             AST_block* code)
{
    if ( 0 == t ) {
        type = NONE;
    } else if ( strcmp(t->type(), "int") == 0 ){
        type = INT;
    } else if ( strcmp(t->type(), "float") == 0 ){
        type = FLOAT;
    } else if ( strcmp(t->type(), "char") == 0 ){
        type = CHAR;
    } else if ( strcmp(t->type(), "boolean") == 0 ){
        type = BOOLEAN;
    } else {
        throw "Error en implementación de identificación de tipo de función";
    }

    line = t->line;
    column = t->column;
    free(t);

    identifier = string(id->ident);
    line_id = id->line;
    column_id = id->column;
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

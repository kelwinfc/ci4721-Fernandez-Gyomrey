/* 
 * Constructores del tipo AST
 * 
 */

#include "AST.h"
#include "AST/dump.cpp"
#include "AST/check.cpp"
#include "AST/constant_folding.cpp"
#include "AST/gen_tac.cpp"

AST_op::AST_op(AST_expression* l, tokenId* o, AST_expression* r){
    
    is_constant = false;
    type = UNDEFINED;
    
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
    } else if ( o->ident.compare("%") == 0 ){
        oper_type = MOD;
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
    delete o;
    
    left = l;
    right = r;
}

AST_un_op::AST_un_op(tokenId* o, AST_expression* e){
    
    is_constant = false;
    type = UNDEFINED;
    
    if ( o->ident.compare("!") == 0 ){
        oper_type = NOT;
    } else if ( o->ident.compare("-") == 0 ){
        oper_type = NEG;
    } else {
        throw "Error de implementación en identificación de operador de expresión unaria";
    }
    
    line = o->line;
    column = o->column;
    delete o;

    expr = e;
}

AST_int::AST_int(tokenInt* tk){
    
    type = INT;
    is_constant = true;
    
    line = tk->line;
    column = tk->column;
    value = tk->number;
    delete tk;
}

AST_float::AST_float(tokenFloat* tk){

    type = FLOAT;
    is_constant = true;
    
    line = tk->line;
    column = tk->column;
    value = tk->number;
    delete tk;
}

AST_boolean::AST_boolean(tokenBoolean* tk){
    
    type = BOOLEAN;
    is_constant = true;
    
    line = tk->line;
    column = tk->column;
    value = tk->value;
    delete tk;
}

AST_char::AST_char(tokenId* tk){

    type = CHAR;
    is_constant = true;
    
    line = tk->line;
    column = tk->column;
    value = tk->ident + "";
    value = value.substr(1, value.length() - 2);
    delete tk;
}

AST_string::AST_string(tokenId* tk){

    is_constant = true;
    
    line = tk->line;
    column = tk->column;
    value = tk->ident + "";

    value = value.substr(1, value.length() - 2);
    type = STRING;
    delete tk;
}

AST_enum_constant::AST_enum_constant(tokenConstant* tk){

    is_constant = true;
    
    line = tk->line;
    column = tk->column;
    value = tk->ident;

    type = tk->type;
    delete tk;
}

void AST_string::append(tokenId* tk){
    is_constant = true;
    string c = tk->ident;
    value += c.substr(1, c.length() - 2);
    delete tk;
}

AST_ident::AST_ident(tokenId* tk){

    type = UNDEFINED;
    is_constant = false;
    
    line = tk->line;
    column = tk->column;
    value = string(tk->ident);
    delete tk;
}

AST_dereference::AST_dereference(AST_lval* v){
    is_constant = false;
    value = v;
    line = v->line;
    column = v->column;
}

AST_address::AST_address(AST_lval* v){
    is_constant = false;
    value = v;
    line = v->line;
    column = v->column;
}

AST_array_access::AST_array_access(AST_lval* lvalue, AST_expression* ind ){
    is_constant = false;
    value = lvalue;
    line = lvalue->line;
    column = lvalue->column;
    
    index = ind;
}

AST_struct_access::AST_struct_access( AST_lval* lvalue, tokenId* f){
    is_constant = false;
    value = lvalue;
    line = f->line;
    column = f->column;
    
    field = f->ident;
    
    delete f;
}

AST_rlval::AST_rlval( AST_lval* lvalue ){
    value = lvalue;
    line = lvalue->line;
    column = lvalue->column;
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
    
    is_constant = false;
    returns_value = true;// en principio se asume que sí. AST_procedure lo quitará de hacer falta
    
    line = tk->line;
    column = tk->column;
    name = string(tk->ident);
    delete tk;
    
    params = p;
    has_return = NO;
}

AST_conversion::AST_conversion(TYPE t, AST_expression* e){
    
    line = e->line;
    column = e->column;
    
    this->type = t;
    this->expr = e;
    this->original_type = UNDEFINED;
    
    is_constant = e->is_constant;
    
}

AST_variable_declaration::AST_variable_declaration(TYPE t, tokenId* id,
                                                   AST_expression* v,
                                                   bool constant
                                                  )
{
    line = id->line;
    column = id->column;

    sym = new symbol(string(id->ident), constant, t, id->line,
                     id->column, 0 != v
                    );
    
    delete id;
    
    value = v;
    has_return = NO;
}

AST_block::AST_block(int l, int c){
    line = l;
    column = c;
    statements.clear();
    has_return = NO;
}

void AST_block::add_statement(AST_statement* s){

    if ( statements.size() != 0 ){
        if ( statements.back()->has_return == YES ){
            if (    ( statements.size() > 1 
                      && statements[statements.size() - 2]->has_return != YES)
                 || ( statements.size() == 1 )
               )
            {
                logger->buf << "Codigo inalcanzable";
                logger->warning(s->line, s->column);
            }
            s->has_return = YES;
        } else if (    statements.back()->has_return == MAYBE 
                    && s->has_return != YES )
        {
            s->has_return = MAYBE;
        }
    }

    has_return = s->has_return;

    statements.push_back(s);
}

AST_arg_list::AST_arg_list(){

}

void AST_arg_list::add_argument( TYPE t, tokenId* id, bool constant ){
    symbol* next_arg = new symbol(id->ident, constant, t,
                                  id->line, id->column, false);
    
    args.push_back( next_arg );
    
    line = args[0]->getLine();
    
    delete id;
}

AST_discrete_arg_list::AST_discrete_arg_list(){

}

void AST_discrete_arg_list::add_argument( TYPE t, tokenId* id,
                                          int min_value, int max_value,
                                          bool constant )
{
    symbol* next_arg = new symbol(id->ident, constant, t,
                                  id->line, id->column, false);
    
    args.push_back( next_arg );
    boundaries.push_back( pair<int,int>(min_value, max_value) );
    
    line = args[0]->getLine();
    
    delete id;
    
}

AST_function::AST_function(TYPE t, tokenId* id, AST_arg_list* args,
                           AST_block* code
                          )
{
    line = id->line;
    column = id->column;
    
    vector< symbol* >::iterator it;
    vector< TYPE > types;
    for (it = args->args.begin(); it != args->args.end(); ++ it){
        types.push_back( (*it)->getType() );
    }
    
    if ( t ){
        func = new symbol_function(id->ident, t, id->line, id->column,
                                   types);
    } else {
        func = new symbol_function(id->ident, id->line, id->column, types );
    }
    
    delete id;
    
    formal_parameters = args;
    instructions = code;
    
    if ( func->getType() != NONE ){
        if ( code->has_return == NO ){
            // Reportar error de tipo de argumento
            logger->buf << "Función '" << id->ident << "' sin retorno.";
            logger->error(line, column);
        } else if ( code->has_return == MAYBE ){
            logger->buf << "Función '" << id->ident << "' posiblemente sin retorno.";
            logger->warning(line, column);
        }
    }
    has_return = NO;
    
}

AST_program::AST_program(){

}

void AST_program::add_declaration(AST_declaration* d){
    declarations.push_back(d);
}

AST_assignment::AST_assignment(AST_lval* l, AST_expression* e){

    lvalue = l;
    line = l->line;
    column = l->column;
    
    expr = e;
    has_return = NO;
}

AST_procedure_call::AST_procedure_call(tokenId* tk, AST_parameters_list* p){
    funcall = new AST_function_call(tk, p);
    funcall->returns_value = false;
    line = funcall->line;
    column = funcall->column;
    has_return = NO;
}

AST_return::AST_return(token* tk, AST_expression* e){

    line = tk->line;
    column = tk->column;
    delete tk;

    expr = e;
    has_return = YES;
}

AST_conditional::AST_conditional(token* tk, AST_expression* e, AST_block* b,
                                 AST_conditional* branches)
{
    line = tk->line;
    column = tk->column;
    delete tk;

    expr = e;
    blck = b;
    else_if = branches;
    
    has_return = b->has_return;
    
    if ( branches ){
        if ( has_return == YES ){
            if ( branches->has_return == NO ){
                has_return = MAYBE;
            } else {
                has_return = branches->has_return;
            }
        } else if ( has_return == NO ){
            if ( branches->has_return != NO ){
                has_return = MAYBE;
            }
        } else {
            has_return = MAYBE;
        }
    } else if ( !e ) {
        has_return = b->has_return;
    } else if ( b->has_return != NO ){
        has_return = MAYBE;
    }
}

AST_loop::AST_loop(token* tk, AST_expression* e, AST_block* b)
{
    line = tk->line;
    column = tk->column;
    delete tk;

    expr = e;
    blck = b;
    
    if ( blck->has_return != NO ){
        has_return = MAYBE;
    } else {
        has_return = NO;
    }
}

AST_bounded_loop::AST_bounded_loop(token* for_, tokenId* id,
                                   AST_expression* l,
                                   AST_expression* r,
                                   AST_block* b)
{
    line = for_->line;
    column = for_->column;
    delete for_;

    name = string(id->ident);
    line_name = id->line;
    column_name = id->column;
    delete id;
    
    left_bound = l;
    right_bound = r;
    
    sym = 0;
    blck = b;
    
    if ( blck->has_return != NO ){
        has_return = MAYBE;
    } else {
        has_return = NO;
    }
}

AST_break::AST_break(token* t){
    line = t->line;
    column = t->column;
    delete t;
    
    has_return = NO;
}

AST_continue::AST_continue(token* t){
    line = t->line;
    column = t->column;
    delete t;
    
    has_return = NO;
}

AST_read::AST_read(tokenId* t){
    line = t->line;
    column = t->line;
    
    variable = string(t->ident);
    delete t;
    
    has_return = NO;
}

AST_print::AST_print(){
    has_return = NO;
}

void AST_print::add_argument(AST_expression* e){
    list.push_back(e);
    
    if ( list.size() == 1 ){
        line   = e->line;
        column = e->column;
    }
    has_return = NO;
}

AST_fill::AST_fill(AST_expression* e, tokenId* f){
    line = e->line;
    column = e->line;
    
    expr = e;
    function = string(f->ident);
    sym = 0;
    delete f;
    
    has_return = NO;
}

AST_map::AST_map(AST_expression* s, AST_expression* d, tokenId* f){
    line = s->line;
    column = s->line;
    
    src = s;
    dst = d;
    
    function = string(f->ident);
    sym = 0;
    delete f;
    
    has_return = NO;
}

bool AST_expression::has_functions(){
    return false; 
}

bool AST_op::has_functions(){
    return left->has_functions() || right->has_functions();
}

bool AST_un_op::has_functions(){
    return expr->has_functions(); 
}

bool AST_function_call::has_functions(){
    return true;
}

bool AST_conversion::has_functions(){
    return expr->has_functions();
}


void AST_statement::gen_tac(block *b){
    //TODO throw exception. se supone que no debería usarse este método
}

opd *AST_expression::gen_tac(block *b){
    return 0;
}

opd *AST_lval::gen_tac(block *b){
    return 0;
}

opd *AST_op::gen_tac(block *b){
    return 0;
}

opd *AST_un_op::gen_tac(block *b){
    return 0;
}

opd *AST_int::gen_tac(block *b){
    return new opd(value);
}

opd *AST_float::gen_tac(block *b){
    return 0;
}

opd *AST_char::gen_tac(block *b){
    return 0;
}

opd *AST_string::gen_tac(block *b){
    return 0;
}

opd *AST_enum_constant::gen_tac(block *b){
    return 0;
}

opd *AST_boolean::gen_tac(block *b){
    return 0;
}

opd *AST_ident::gen_tac(block* b){
    return 0;
}

opd *AST_dereference::gen_tac(block *b){
    return 0;
}

opd *AST_address::gen_tac(block *b){
    return 0;
}

opd *AST_array_access::gen_tac(block *b){
    return 0;
}

opd *AST_struct_access::gen_tac(block *b){
    return 0;
}

opd *AST_conversion::gen_tac(block *b){
    return 0;
}

void AST_block::gen_tac(block *b){

    uint nsize = statements.size();
    for ( uint i = 0; i != nsize; i++){
        statements[i]->gen_tac(b);
    }
}

void AST_parameters_list::gen_tac(block* b){

}

opd *AST_function_call::gen_tac(block *b){
    return 0;
}

void AST_declaration::gen_tac(block *b){
    //TODO throw exception. se supone que no debería usarse este método
}

void AST_variable_declaration::gen_tac(block *b){
    if (0 != value) {
        b->append_inst(new quad(quad::CP, new opd(sym), value->gen_tac(b)));
    }
}

void AST_arg_list::gen_tac(block *b){

}

void AST_discrete_arg_list::gen_tac(block *b){

}

void AST_function::gen_tac(block *b){

    formal_parameters->gen_tac(b);
    instructions->gen_tac(b);
}

void AST_program::gen_tac(block *b){

    AST_declaration* m = 0;
    uint nsize = declarations.size();
    for ( uint i = 0; i != nsize; i++){
        if (   typeid(*declarations[i]) == typeid(AST_function)
            && ((AST_function*)declarations[i])->func->getName().compare("main") == 0) {

            m = declarations[i];
        } else {
            declarations[i]->gen_tac(b);
        }
    }
    printf("imprimiendo main\n");
    m->gen_tac(b);
}

void AST_assignment::gen_tac(block *b){

}

void AST_return::gen_tac(block *b){

}

void AST_conditional::gen_tac(block *b){

}

void AST_loop::gen_tac(block *b){

}

void AST_bounded_loop::gen_tac(block *b){

}

void AST_break::gen_tac(block *b){

}

void AST_continue::gen_tac(block *b){

}

void AST_read::gen_tac(block *b){

}

void AST_print::gen_tac(block *b){

}

void AST_fill::gen_tac(block *b){

}

void AST_map::gen_tac(block *b){

}
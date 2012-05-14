
void AST_statement::gen_tac(block *b){
    // Unused
}

opd *AST_expression::gen_tac(block *b){
    // Unused
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
    return new opd(value);
}

opd *AST_char::gen_tac(block *b){
    return new opd(value[0]);
}

opd *AST_string::gen_tac(block *b){
    // TODO en función de una tabla realizar la asignación del apuntador
    // al string. sólo se requiere el valor completo del string al imprimir
    return 0;
}

opd *AST_enum_constant::gen_tac(block *b){
    // TODO las constantes deben tener un índice. esto pasa a ser un número
    // a bajo nivel.
    return 0;
}

opd *AST_boolean::gen_tac(block *b){
    return new opd(value);
}

opd *AST_ident::gen_tac(block* b){
    return new opd(sym);
}

opd *AST_dereference::gen_tac(block *b){
    opd *t = new opd(), *v = value->gen_tac(b);
    b->append_inst(new quad(quad::DEREF, t, v));
    return t;
}

opd *AST_address::gen_tac(block *b){
    opd *t = new opd(), *v = value->gen_tac(b);
    b->append_inst(new quad(quad::REF, t, v));
    return t;
}

opd *AST_array_access::gen_tac(block *b){
    opd *var = value->gen_tac(b),
        *ind = index->gen_tac(b),
        *t = new opd();

    b->append_inst(new quad(quad::LD, t, var, ind));
    return t;
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
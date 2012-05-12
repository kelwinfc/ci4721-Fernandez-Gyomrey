

void AST_node::gen_tac(block *b){

}

void AST_statement::gen_tac(block *b){

}

void AST_expression::gen_tac(block *b){

}

void AST_lval::gen_tac(block *b){

}

void AST_op::gen_tac(block *b){

}

void AST_un_op::gen_tac(block *b){

}

void AST_int::gen_tac(block *b){

}

void AST_float::gen_tac(block *b){

}

void AST_char::gen_tac(block *b){

}

void AST_string::gen_tac(block *b){

}

void AST_enum_constant::gen_tac(block *b){

}

void AST_boolean::gen_tac(block *b){

}

void AST_ident::gen_tac(block* b){

}

void AST_dereference::gen_tac(block *b){

}

void AST_address::gen_tac(block *b){

}

void AST_array_access::gen_tac(block *b){

}

void AST_struct_access::gen_tac(block *b){

}

void AST_conversion::gen_tac(block *b){

}

void AST_block::gen_tac(block *b){

}

void AST_parameters_list::gen_tac(block* b){

}

void AST_function_call::gen_tac(block *b){

}

void AST_declaration::gen_tac(block *b){

}

void AST_variable_declaration::gen_tac(block *b){

}

void AST_arg_list::gen_tac(block *b){

}

void AST_discrete_arg_list::gen_tac(block *b){

}

void AST_function::gen_tac(block *b){

}

void AST_program::gen_tac(block *b){

    AST_declaration* m;
    uint nsize = declarations.size();
    for ( uint i = 0; i != nsize; i++){
        if ("main" == ) {
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

void AST_node::dump(ostream &strm, int indentation){
    throw "INVALID CALL TO void AST_node::dump(ostream &strm, int indentation)";
}

void AST_node::dump_indentation(ostream &strm, int k){
    for (int i=0; i<k; i++){
        strm << "    ";
    }
}

void AST_conditional::dump(ostream &strm, int indentation){
    dump(strm, indentation, true);
}

void AST_break::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Break" << endl;
}

void AST_continue::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Continue" << endl;
}

void AST_declaration::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
}

void AST_variable_declaration::dump(ostream &strm, int indentation){
    
    AST_declaration::dump(strm, indentation);
    
    strm << "Declaration: " << sym->getName()
         << " ("
         << types.types[ sym->getType() ]->name
         << ") [offset "
         << sym->offset
         << "] ";
    
    if ( value != 0 ){
        strm << "initialized with" << endl;
        value->dump(strm, indentation+1);
    } else {
        strm << "unitialized" << endl;
    }
}

void AST_function::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Declaration of Function " 
         << func->getName()
         << " -> "
         << types.types[func->getType()]->name
         << endl;
    
    strm << setw(3)
         << (formal_parameters->args.size() ? formal_parameters->line : line)
         << ":     (" << endl;
    dump_indentation(strm, indentation);
    
    formal_parameters->dump(strm, indentation+1);

    strm << setw(3)
         << (formal_parameters->args.size() ?
            formal_parameters->args.back()->getLine() : line)
         << ":     )" << endl;
    
    instructions->dump(strm, indentation+1);
}

void AST_arg_list::dump(ostream &strm, int indentation){  
    vector<symbol*>::iterator it;
    for ( it = args.begin(); it != args.end(); ){
        strm << setw(3) << (*it)->getLine() << ": ";
        dump_indentation(strm, indentation);

        strm << types.types[(*it)->getType()]->name << "(";
        strm << (*it)->getName();
        strm << ")";
        
        if ( ++it != args.end() ){
            strm << ",";
        }
        strm << endl;
        
    }
}

void AST_discrete_arg_list::dump(ostream &strm, int indentation){  
    vector<symbol*>::iterator it;
    vector<pair<int,int> >::iterator it_boundaries;
    
    it_boundaries = boundaries.begin();
    for ( it = args.begin(); it != args.end(); ++it_boundaries){
        strm << setw(3) << (*it)->getLine() << ": ";
        dump_indentation(strm, indentation);

        strm << types.types[(*it)->getType()]->name << "(";
        strm << (*it)->getName();
        strm << ") in [" << (*it_boundaries).first << ", "
             << (*it_boundaries).second << "]";
        
        if ( ++it != args.end() ){
            strm << ",";
        }
        
        strm << endl;
        
    }
}

void AST_statement::dump(ostream &strm, int indentation){
    
}

void AST_block::dump(ostream &strm, int indentation){
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "{" << endl;
    
    vector<AST_statement*>::iterator it;
    for (it = statements.begin(); it != statements.end(); ++ it){
        (*it)->dump(strm, indentation+1);
    }
    
    strm << setw(3) << (statements.size() ? statements.back()->line : line) << ": ";
    dump_indentation(strm, indentation);
    strm << "}" << endl;
}


void AST_expression::dump(ostream &strm, int indentation){
    dump_indentation(strm, indentation);
    strm << "Expression" << endl;
}

void AST_lval::dump(ostream &strm, int indentation){
    dump_indentation(strm, indentation);
    strm << "L-Value" << endl;
}

void AST_rlval::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "R-L-Value:" << endl;
    value->dump(strm, indentation+1);
}

void AST_dereference::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Dereference [" << types.types[type]->name << "]" << endl;
    value->dump(strm, indentation+1);
}

void AST_address::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Pointer to:" << endl;
    value->dump(strm, indentation+1);
}

void AST_array_access::dump(ostream &strm, int indentation){
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    if (index == 0){
        strm << "Error in array access" << endl;
        return;
    }
    
    strm << "Get: [" << types.types[type]->name << "]" << endl;
    
    // si hay un error, no habría índice
    if (0 != index) {
        index->dump(strm, indentation+1);
    }
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "From:" << endl;
    
    value->dump(strm, indentation+1);
}

void AST_struct_access::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Get " << field << "[" << types.types[type]->name << "] ";
    strm << "from:" << endl;
    
    value->dump(strm, indentation+1);
}

string AST_op::binary_operator(){
    switch ( oper_type ){
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case PROD:
            return "*";
        case DIV:
            return "/";
        case MOD:
            return "%";
        case AND:
            return "&&";
        case OR:
            return "||";
        case IMP:
            return "==>";
        case CONSEQ:
            return "<==";
        case EQ:
            return "==";
        case UNEQ:
            return "!=";
        case LESS:
            return "<";
        case LESS_EQ:
            return "<=";
        case GREAT:
            return ">";
        case GREAT_EQ:
            return ">=";
    }
    return "";
}

void AST_op::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << binary_operator() << " [" << types.types[type]->name << "]" << endl;
    
    left->dump(strm, indentation+1);
    right->dump(strm, indentation+1);
    
}

string AST_un_op::unary_operator(){
    
    switch ( oper_type ){
        case NOT:
            return "!";
        case NEG:
            return "-";
    }
}

void AST_un_op::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Unary " << unary_operator()
         << " [" << types.types[type]->name << "]" << endl;
    
    expr->dump(strm, indentation+1);
    
}

void AST_int::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Constant int: " << value << endl;
}

void AST_float::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Constant float: " << value << endl;
}

void AST_boolean::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Constant boolean: " << (value ? "true" : "false") << endl;
}

void AST_char::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Constant char: " << value << endl;
}

void AST_string::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Constant string: " << value << endl;
}

void AST_enum_constant::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Constant enum: " << value << endl;
}

void AST_ident::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Identifier: " << value;
    if ( sym ){
        strm << " ["
             << types.types[sym->getType()]->name
             << ":"
             << sym->getLine()
             << ":"
             << sym->getColumn()
             << "]";
    } else {
        strm << " [UNDEFINED]";
    }
    strm << endl;
}

void AST_function_call::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << name;
    if ( sym ){
        strm << " ["
             << types.types[sym->getType()]->name
             << ":"
             << sym->getLine()
             << ":"
             << sym->getColumn()
             << "]";
    } else {
        strm << " [UNDEFINED]";
    }
    
    strm << endl;
    strm << "     ";
    dump_indentation(strm, indentation);
    
    strm << "(" << endl;
    params->dump(strm, indentation+1);
    
    strm << "     ";
    dump_indentation(strm, indentation);
    strm << ")" << endl;
}

void AST_parameters_list::dump(ostream &strm, int indentation){
    
    for (uint i=0; i < elem.size(); i++){
        if (0 != elem[i]) {
            elem[i]->dump(strm, indentation);
        }
        
        if ( i + 1 < elem.size() ){
            strm << "   ";
            dump_indentation(strm, indentation);
            strm << "," << endl;
        }
    }
}

void AST_conversion::dump(ostream &strm, int indentation){
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Conversion from " << types.types[original_type]->name
         << " to " << types.types[type]->name << endl;
     
     expr->dump(strm, indentation+1);
}

void AST::dump(ostream &strm, int indentation){
    uint nsize = declarations.size();
    for (uint i=0; i < nsize; i++){
        declarations[i]->dump(strm, 0);
    }
}

void AST_assignment::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Assignment:" << endl;
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation+1);
    
    strm << "Lvalue ["
         << types.types[lvalue->type]->name
         << "]" << endl;
    lvalue->dump(strm, indentation+2);
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation+1);
    
    if (0 != expr) {
        strm << "Rvalue ["
             << types.types[expr->type]->name
             << "]" << endl;
        
        expr->dump(strm, indentation+2);
    } else {
        strm << "Rvalue undefined" << endl;
    }
}

void AST_procedure_call::dump(ostream &strm, int indentation){
    funcall->dump(strm, indentation);
}

void AST_return::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    if ( expr ){
        strm << "Return [" << types.types[expr->type]->name << "]" << endl;
        expr->dump(strm, indentation+1);
    } else {
        strm << "Empty return" << endl;
    }
}

void AST_conditional::dump(ostream &strm, int indentation, bool first){
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    if ( first ){
        strm << "If" << endl;
    } else if ( expr ){
        strm << "Else If" << endl;
    } else {
        strm << "Else" << endl;
    }

    dump_indentation(strm, indentation+1);
    strm << "     (" << endl;
    
    if ( expr ){
        expr->dump(strm, indentation+1);
    }
    
    dump_indentation(strm, indentation+1);
    strm << "     )" << endl;
    
    blck->dump(strm, indentation+1);
    
    if ( else_if ){
        else_if->dump(strm, indentation, false);
    }
}

void AST_loop::dump(ostream &strm, int indentation){
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "While" << endl;
    
    dump_indentation(strm, indentation+1);
    strm << "     (" << endl;
    
    if ( expr ){
        expr->dump(strm, indentation+1);
    }
    
    dump_indentation(strm, indentation+1);
    strm << "     )" << endl;
    
    blck->dump(strm, indentation+1);
}

void AST_bounded_loop::dump(ostream &strm, int indentation){
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "For " << name << " in" << endl;
    
    dump_indentation(strm, indentation+1);
    strm << "     (" << endl;
    
    if (0 != left_bound) {
        left_bound->dump(strm, indentation+2);
    }
    
    dump_indentation(strm, indentation+1);
    strm << "     ," << endl;
    
    if (0 != right_bound) {
        right_bound->dump(strm, indentation+2);
    }
    
    dump_indentation(strm, indentation+1);
    strm << "     )" << endl;
    
    blck->dump(strm, indentation+1);
}

void AST_read::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Read " << variable;
    
    if ( sym ){
        strm << " ["
             << types.types[sym->getType()]->name
             << ":"
             << sym->getLine()
             << ":"
             << sym->getColumn()
             << "]";
    } else {
        strm << " [UNDEFINED]";
    }
    strm << endl;
}

void AST_print::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Print" << endl;
    vector<AST_expression*>::iterator it;
    for (it=list.begin(); it!=list.end(); ++it){
        
        if ( it != list.begin() ){
            dump_indentation(strm, indentation+1);
            strm << "     ," << endl;
        }
        
        (*it)->dump(strm, indentation+1);
    }
}

void AST_fill::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Fill" << endl;
    expr->dump(strm, indentation+1);
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "With " << function << endl;
}

void AST_map::dump(ostream &strm, int indentation){
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    
    strm << "Map (" << function << ")" <<endl;
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Source: ";
    src->dump(strm, indentation+1);
    
    strm << setw(3) << line << ": ";
    dump_indentation(strm, indentation);
    strm << "Destination: ";
    dst->dump(strm, indentation+1);
}

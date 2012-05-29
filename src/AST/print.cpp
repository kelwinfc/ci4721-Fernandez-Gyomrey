/* 
 * Imprime el AST, asume que se ha ejecutado anteriormente el llenado de tipos
 * 
 */

void AST_node::print(int indentation){
    
}

void AST_conditional::print(int indentation){
    print(indentation, true);
}

void AST_break::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Break\n";
}

void AST_continue::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Continue\n";
}

void AST_declaration::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
}

void AST_variable_declaration::print(int indentation){
    
    AST_declaration::print(indentation);
    
    cout << "Declaration: " << sym->getName()
         << " ("
         << types.types[ sym->getType() ]->name
         << ") [offset "
         << sym->offset
         << "] ";
    
    if ( value != 0 ){
        cout << "initialized with\n";
        value->print(indentation+1);
    } else {
        cout << "unitialized\n";
    }
}

void AST_function::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Declaration of Function " 
         << func->getName()
         << " -> "
         << types.types[func->getType()]->name
         << endl;
    
    printf("%3d:     (\n",
           formal_parameters->args.size() ? formal_parameters->line : line
          );
    print_indentation(indentation);
    
    formal_parameters->print(indentation+1);
    
    printf("%3d:     )\n",
           formal_parameters->args.size() ? 
                 formal_parameters->args.back()->getLine()
               : line
          );
    
    instructions->print(indentation+1);
}

void AST_arg_list::print(int indentation){  
    vector<symbol*>::iterator it;
    for ( it = args.begin(); it != args.end(); ){
        printf("%3d: ", (*it)->getLine());
        print_indentation(indentation);

        cout << types.types[(*it)->getType()]->name << "(";
        cout << (*it)->getName();
        cout << ")";
        
        if ( ++it != args.end() ){
            cout << ",";
        }
        cout << endl;
        
    }
}

void AST_discrete_arg_list::print(int indentation){  
    vector<symbol*>::iterator it;
    vector<pair<int,int> >::iterator it_boundaries;
    
    it_boundaries = boundaries.begin();
    for ( it = args.begin(); it != args.end(); ++it_boundaries){
        printf("%3d: ", (*it)->getLine());
        print_indentation(indentation);

        cout << types.types[(*it)->getType()]->name << "(";
        cout << (*it)->getName();
        cout << ") in [" << (*it_boundaries).first << ", "
             << (*it_boundaries).second << "]";
        
        if ( ++it != args.end() ){
            cout << ",";
        }
        
        cout << endl;
        
    }
}

void AST_statement::print(int indentation){
    
}

void AST_block::print(int indentation){
    
    printf("%3d: ", line );
    print_indentation(indentation);
    cout << "{\n";
    
    vector<AST_statement*>::iterator it;
    for (it = statements.begin(); it != statements.end(); ++ it){
        (*it)->print(indentation+1);
    }
    
    printf("%3d: ",statements.size() ? statements.back()->line : line );
    print_indentation(indentation);
    cout << "}\n";
}


void AST_expression::print(int indentation){
    print_indentation(indentation);
    cout << "Expression\n";
}

void AST_lval::print(int indentation){
    print_indentation(indentation);
    cout << "L-Value\n";
}

void AST_dereference::print(int indentation){
    printf("%3d: ", line );
    print_indentation(indentation);
    cout << "Dereference [" << types.types[type]->name << "]\n";
    value->print(indentation+1);
}

void AST_address::print(int indentation){
    printf("%3d: ", line );
    print_indentation(indentation);
    cout << "Pointer to:\n";
    value->print(indentation+1);
}

void AST_array_access::print(int indentation){
    
    printf("%3d: ", line );
    print_indentation(indentation);
    
    if (index == 0){
        cout << "Error in array access\n";
        return;
    }
    
    cout << "Get: [" << types.types[type]->name << "]\n";
    
    // si hay un error, no habr�a �ndice
    if (0 != index) {
        index->print(indentation+1);
    }
    
    printf("%3d: ", line );
    print_indentation(indentation);
    cout << "From:\n";
    
    value->print(indentation+1);
}

void AST_struct_access::print(int indentation){
    printf("%3d: ", line );
    print_indentation(indentation);
    cout << "Get " << field << "[" << types.types[type]->name << "] ";
    cout << "from:\n";
    
    value->print(indentation+1);
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

void AST_op::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << binary_operator() << " [" << types.types[type]->name << "]\n";
    
    left->print(indentation+1);
    right->print(indentation+1);
    
}

string AST_un_op::unary_operator(){
    
    switch ( oper_type ){
        case NOT:
            return "!";
        case NEG:
            return "-";
    }
}

void AST_un_op::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Unary " << unary_operator()
         << " [" << types.types[type]->name << "]\n";
    
    expr->print(indentation+1);
    
}

void AST_int::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Constant int: " << value << endl;
}

void AST_float::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Constant float: " << value << endl;
}

void AST_boolean::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Constant boolean: " << (value ? "true" : "false") << endl;
}

void AST_char::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Constant char: " << value << endl;
}

void AST_string::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Constant string: " << value << endl;
}

void AST_enum_constant::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Constant enum: " << value << endl;
}

void AST_ident::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Identifier: " << value;
    if ( sym ){
        cout << " ["
             << types.types[sym->getType()]->name
             << ":"
             << sym->getLine()
             << ":"
             << sym->getColumn()
             << "]";
    } else {
        cout << " [UNDEFINED]";
    }
    cout << endl;
}

void AST_function_call::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << name;
    if ( sym ){
        cout << " ["
             << types.types[sym->getType()]->name
             << ":"
             << sym->getLine()
             << ":"
             << sym->getColumn()
             << "]";
    } else {
        cout << " [UNDEFINED]";
    }
    
    cout << endl;
    cout << "     ";
    print_indentation(indentation);
    
    cout << "(" << endl;
    params->print(indentation+1);
    
    cout << "     ";
    print_indentation(indentation);
    cout << ")\n";
}

void AST_parameters_list::print(int indentation){
    
    for (uint i=0; i < elem.size(); i++){
        if (0 != elem[i]) {
            elem[i]->print(indentation);
        }
        
        if ( i + 1 < elem.size() ){
            cout << "   ";
            print_indentation(indentation);
            cout << ",\n";
        }
    }
}

void AST_conversion::print(int indentation){
    
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Conversion from " << types.types[original_type]->name
         << " to " << types.types[type]->name << endl;
     
     expr->print(indentation+1);
}

void AST_program::print(int indentation){
    uint nsize = declarations.size();
    for (uint i=0; i < nsize; i++){
        declarations[i]->print(0);
    }
}

void AST_assignment::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Assignment:\n";
    
    printf("%3d: ", line);
    print_indentation(indentation+1);
    
    cout << "Lvalue ["
         << types.types[lvalue->type]->name
         << "]\n";
    lvalue->print(indentation+2);
    
    printf("%3d: ", line);
    print_indentation(indentation+1);
    
    if (0 != expr) {
        cout << "Rvalue ["
             << types.types[expr->type]->name
             << "]\n";
        
        expr->print(indentation+2);
    } else {
        cout << "Rvalue undefined" << endl;
    }
}


void AST_return::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    if ( expr ){
        cout << "Return [" << types.types[expr->type]->name << "]" << endl;
        expr->print(indentation+1);
    } else {
        cout << "Empty return" << endl;
    }
}

void AST_conditional::print(int indentation, bool first){
    
    printf("%3d: ", line);
    print_indentation(indentation);
    
    if ( first ){
        cout << "If\n";
    } else if ( expr ){
        cout << "Else If\n";
    } else {
        cout << "Else\n";
    }
    
    print_indentation(indentation+1);
    cout << "     (\n";
    
    if ( expr ){
        expr->print(indentation+1);
    }
    
    print_indentation(indentation+1);
    cout << "     )\n";
    
    blck->print(indentation+1);
    
    if ( else_if ){
        else_if->print(indentation, false);
    }
}

void AST_loop::print(int indentation){
    
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "While\n";
    
    print_indentation(indentation+1);
    cout << "     (\n";
    
    if ( expr ){
        expr->print(indentation+1);
    }
    
    print_indentation(indentation+1);
    cout << "     )\n";
    
    blck->print(indentation+1);
}

void AST_bounded_loop::print(int indentation){
    
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "For " << name << " in" << endl;
    
    print_indentation(indentation+1);
    cout << "     (\n";
    
    if (0 != left_bound) {
        left_bound->print(indentation+2);
    }
    
    print_indentation(indentation+1);
    cout << "     ,\n";
    
    if (0 != right_bound) {
        right_bound->print(indentation+2);
    }
    
    print_indentation(indentation+1);
    cout << "     )\n";
    
    blck->print(indentation+1);
}

void AST_read::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Read " << variable;
    
    if ( sym ){
        cout << " ["
             << types.types[sym->getType()]->name
             << ":"
             << sym->getLine()
             << ":"
             << sym->getColumn()
             << "]";
    } else {
        cout << " [UNDEFINED]";
    }
    cout << endl;
}

void AST_print::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Print" << endl;
    vector<AST_expression*>::iterator it;
    for (it=list.begin(); it!=list.end(); ++it){
        
        if ( it != list.begin() ){
            print_indentation(indentation+1);
            cout << "     ,\n";
        }
        
        (*it)->print(indentation+1);
    }
}

void AST_fill::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Fill" << endl;
    expr->print(indentation+1);
    
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "With " << function << endl;
}

void AST_map::print(int indentation){
    printf("%3d: ", line);
    print_indentation(indentation);
    
    cout << "Map (" << function << ")" <<endl;
    
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Source: ";
    src->print(indentation+1);
    
    printf("%3d: ", line);
    print_indentation(indentation);
    cout << "Destination: ";
    dst->print(indentation+1);
}

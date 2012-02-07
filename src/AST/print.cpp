#include "AST.h"

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


void AST_variable_declaration::print(int indentation){
    AST_declaration::print(indentation);
    
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
    cout << "Declaration of Function " << identifier << " -> ";
    
    switch ( func->getType() ) {
        case symbol::INT:
            cout << "int";
        case symbol::FLOAT:
            cout << "float";
        case symbol::CHAR:
            cout << "char";
        case symbol::BOOLEAN:
            cout << "boolean";
        case symbol::NONE:
            cout << "none";
        case symbol::INVALID:
            cout << "invalid";
        case symbol::UNDEFINED:
            cout << "undefined";
    }
    cout << endl;
    
    cout << "     (\n";
    //formal_parameters->print(indentation+1);
    cout << "     )\n";
    instructions->print(indentation+1);
}

void AST_arg_list::print(int indentation){/*
    for (int i = 0; i < args.size(); ++ i){

        print_indentation(indentation);

        cout << args[i].second << "(";
        args[i].first->print();
        cout << ")";

        if ( i ){
            cout << ",";
        }
        cout << endl;
    }*/
}

void AST_statement::print(int indentation){/*
    print_indentation(indentation+2);
    cout << "bello statement\n";*/
}

void AST_block::print(int indentation){/*
    print_indentation(indentation);
    cout << "     {\n";
    for (int i=statements.size()-1; i >= 0; i--){
        statements[i]->print(indentation+1);
    }
    print_indentation(indentation);
    cout << "     }\n";*/
}


void AST_expression::print(int indentation){/*
    print_indentation(indentation);
    cout << "Expression\n";*/
}

void AST_op::print(int indentation){/*
    printf("%3d: ", line_op);
    print_indentation(indentation);
    cout << op->ident << endl;
    
    left->print(indentation+1);
    right->print(indentation+1);*/
    
}

void AST_un_op::print(int indentation){/*
    printf("%3d: ", op->line);
    print_indentation(indentation);
    cout << "Unary " << op->ident << endl;
    
    expr->print(indentation+1);*/
    
}

void AST_int::print(int indentation){/*
    printf("%3d: ", value->line);
    print_indentation(indentation);
    cout << "Constant int: " << value->number << endl;*/
}

void AST_float::print(int indentation){/*
    printf("%3d: ", value->line);
    print_indentation(indentation);
    cout << "Constant float: " << value->number << endl;*/
}

void AST_boolean::print(int indentation){/*
    printf("%3d: ", value->line);
    print_indentation(indentation);
    cout << "Constant boolean: " << (value->value ? "true" : "false") << endl;*/
}

void AST_char::print(int indentation){/*
    printf("%3d: ", value->line);
    print_indentation(indentation);
    cout << "Constant char: " << value->ident << endl;*/
}

void AST_ident::print(int indentation){/*
    printf("%3d: ", value->line);
    print_indentation(indentation);
    cout << "Identifier: " << value->ident << endl;*/
}

void AST_function_call::print(int indentation){/*
    printf("%3d: ", ident->line);
    print_indentation(indentation);
    
    cout << ident->ident << endl;
    
    cout << "     ";
    print_indentation(indentation);
    
    cout << "(" << endl;
    params->print(indentation+1);
    
    cout << "     ";
    print_indentation(indentation);
    cout << ")\n";*/
}

void AST_parameters_list::print(int indentation){/*
    for (int i=elem.size()-1; i >= 0; i--){
        elem[i]->print(indentation);
        
        if ( i != 0 ){
            cout << "   ";
            print_indentation(indentation);
            cout << "...\n";
        }
    }*/
}

void AST_program::print(int indentation){/*
    for (int i=declarations.size()-1; i >= 0; i--){
        declarations[i]->print(0);
    }*/
}

void AST_assignment::print(int indentation){/*
    printf("%3d: ", variable->line);
    print_indentation(indentation);
    
    cout << "Assignment " << variable->ident << " <--\n";
    
    expr->print(indentation+1);*/
}

void AST_return::print(int indentation){/*
    printf("%3d: ", ret->line);
    print_indentation(indentation);
    
    if ( expr ){
        cout << "Return" << endl;
        expr->print(indentation+1);
    } else {
        cout << "Empty return" << endl;
    }*/
}

void AST_conditional::print(int indentation, bool first){/*
    
    printf("%3d: ", tk_if->line);
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
    
    block->print(indentation+1);
    
    if ( else_if ){
        else_if->print(indentation, false);
    }*/
}

void AST_loop::print(int indentation){/*
    
    printf("%3d: ", tk_while->line);
    print_indentation(indentation);
    
    cout << "While\n";
    
    print_indentation(indentation+1);
    cout << "     (\n";
    
    if ( expr ){
        expr->print(indentation+1);
    }
    
    print_indentation(indentation+1);
    cout << "     )\n";
    
    block->print(indentation+1);*/
}

void AST_bounded_loop::print(int indentation){/*
    
    printf("%3d: ", tk_for->line);
    print_indentation(indentation);
    
    cout << "For " << tk_ident->ident << " in" << endl;
    
    print_indentation(indentation+1);
    cout << "     (\n";
    
    left_bound->print(indentation+1);
    
    print_indentation(indentation+1);
    cout << "     ,\n";
    
    right_bound->print(indentation+1);
    
    print_indentation(indentation+1);
    cout << "     )\n";
    
    block->print(indentation+1);*/
}
/*
void AST_type::print(int indentation){
    switch (type) {
        case AST_type::INT:
            cout << "int";
            break;
        case AST_type::FLOAT:
            cout << "float";
            break;
        case AST_type::BOOLEAN:
            cout << "boolean";
            break;
        case AST_type::CHAR:
            cout << "char";
            break;
        default:
            break;
    }
}*/
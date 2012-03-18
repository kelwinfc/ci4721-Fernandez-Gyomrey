AST_expression* AST_op::constant_folding(){
    
    if (    !left->is_constant
         || !right->is_constant 
         || (typeid(*left) != typeid(*right))
       )
    {
        return this;
    }
    
    bool equals = false;
    
    switch( oper_type ){
        
        case MOD:
            if ( typeid(*left) == typeid(AST_int) ){
                tokenInt* tk = new tokenInt( left->line, left->column,
                                             (char*)"0");
                AST_int* sol = new AST_int(tk);
                sol->value = ((AST_int*)left)->value % ((AST_int*)right)->value;
                
                return sol;
            }
            break;
            
        /* Operadores aritmeticos: suma, resta, multiplicacion y division */
        case PLUS:
        case MINUS:
        case PROD:
        case DIV:
            
            if ( typeid(*left) == typeid(AST_int) ){
                tokenInt* tk = new tokenInt( left->line, left->column,
                                             (char*)"0");
                AST_int* sol = new AST_int(tk);
                switch( oper_type ){
                    case PLUS:
                        sol->value = 
                            ((AST_int*)left)->value + ((AST_int*)right)->value;
                        break;
                    case MINUS:
                        sol->value = 
                            ((AST_int*)left)->value - ((AST_int*)right)->value;
                        break;
                    case PROD:
                        sol->value = 
                            ((AST_int*)left)->value * ((AST_int*)right)->value;
                        break;
                    case DIV:
                        sol->value = 
                            ((AST_int*)left)->value / ((AST_int*)right)->value;
                        break;
                    default:
                        break;
                }
                return sol;
                
            } else if ( typeid(*left) == typeid(AST_float) ){
                tokenFloat* tk = new tokenFloat( left->line, left->column,
                                                 (char*)"0.0");
                AST_float* sol = new AST_float(tk);
                sol->value = ((AST_float*)left)->value + 
                                ((AST_float*)right)->value;
                                
                switch( oper_type ){
                    case PLUS:
                        sol->value = ((AST_float*)left)->value 
                                        + ((AST_float*)right)->value;
                        break;
                    case MINUS:
                        sol->value = ((AST_float*)left)->value 
                                        - ((AST_float*)right)->value;
                        break;
                    case PROD:
                        sol->value = ((AST_float*)left)->value 
                                        * ((AST_float*)right)->value;
                        break;
                    case DIV:
                        sol->value = ((AST_float*)left)->value 
                                        / ((AST_float*)right)->value;
                        break;
                    default:
                        break;
                }
                
                return sol;
            }
            
            break;
        
        /* Operadores logicos: y, o, implica, consecuencia */
        case AND:
        case OR:
        case IMP:
        case CONSEQ:
            
            if ( typeid(*left) == typeid(AST_boolean) ){
                tokenBoolean* tk = new tokenBoolean( left->line, left->column,
                                             (char*)"true");
                AST_boolean* sol = new AST_boolean(tk);
                switch( oper_type ){
                    case AND:
                        sol->value = ((AST_boolean*)left)->value 
                                        && ((AST_boolean*)right)->value;
                        break;
                    case OR:
                        sol->value = ((AST_boolean*)left)->value 
                                        || ((AST_boolean*)right)->value;
                        break;
                    case IMP:
                        sol->value = !((AST_boolean*)left)->value 
                                        || ((AST_boolean*)right)->value;
                        break;
                    case CONSEQ:
                        sol->value = ((AST_boolean*)left)->value 
                                        || !((AST_boolean*)right)->value;
                        break;
                    default:
                        break;
                }
                
                return sol;
            }
        
        /* Operadores de comparación de igualdad: ==, != */
        case EQ:
            equals = true;
        case UNEQ:
            
            {
            tokenBoolean* tk = 
                new tokenBoolean(left->line,left->column,(char*)"true");
            AST_boolean* sol = new AST_boolean(tk);
            
            if ( typeid(*left) == typeid(AST_int) ){
                sol->value = equals == ( ((AST_int*)left)->value 
                                            == ((AST_int*)right)->value );
            } else if ( typeid(*left) == typeid(AST_float) ){
                sol->value = equals == ( ((AST_float*)left)->value 
                                            == ((AST_float*)right)->value );
            } else if ( typeid(*left) == typeid(AST_char) ){
                sol->value = equals == ( ((AST_char*)left)->value 
                                            == ((AST_char*)right)->value );
            } else if ( typeid(*left) == typeid(AST_boolean) ){
                sol->value = equals == ( ((AST_boolean*)left)->value 
                                            == ((AST_boolean*)right)->value );
            } else {
                delete sol;
                return this;
            }
            
            return sol;
            }
            
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
            
            {
            tokenBoolean* tk = 
                new tokenBoolean(left->line,left->column,(char*)"true");
            AST_boolean* sol = new AST_boolean(tk);
            
            if ( typeid(*left) == typeid(AST_int) ){
                sol->value = ((AST_int*)left)->value 
                                            < ((AST_int*)right)->value;
            } else if ( typeid(*left) == typeid(AST_float) ){
                sol->value = ((AST_float*)left)->value 
                                            < ((AST_float*)right)->value;
            } else if ( typeid(*left) == typeid(AST_char) ){
                sol->value = ((AST_char*)left)->value 
                                            < ((AST_char*)right)->value;
            } else {
                delete sol;
                return this;
            }
            
            return sol;
            }
            
        case LESS_EQ:
            
            {
            tokenBoolean* tk = 
                new tokenBoolean(left->line,left->column,(char*)"true");
            AST_boolean* sol = new AST_boolean(tk);
            
            if ( typeid(*left) == typeid(AST_int) ){
                sol->value = ((AST_int*)left)->value 
                                            <= ((AST_int*)right)->value;
            } else if ( typeid(*left) == typeid(AST_float) ){
                sol->value = ((AST_float*)left)->value 
                                            <= ((AST_float*)right)->value;
            } else if ( typeid(*left) == typeid(AST_char) ){
                sol->value = ((AST_char*)left)->value 
                                            <= ((AST_char*)right)->value;
            } else {
                delete sol;
                return this;
            }
            
            return sol;
            }
            
        case GREAT:
            
            {
            tokenBoolean* tk = 
                new tokenBoolean(left->line,left->column,(char*)"true");
            AST_boolean* sol = new AST_boolean(tk);
            
            if ( typeid(*left) == typeid(AST_int) ){
                sol->value = ((AST_int*)left)->value 
                                            > ((AST_int*)right)->value;
            } else if ( typeid(*left) == typeid(AST_float) ){
                sol->value = ((AST_float*)left)->value 
                                            > ((AST_float*)right)->value;
            } else if ( typeid(*left) == typeid(AST_char) ){
                sol->value = ((AST_char*)left)->value 
                                            > ((AST_char*)right)->value;
            } else {
                delete sol;
                return this;
            }
            
            return sol;
            }
            
        case GREAT_EQ:
            
            {
            tokenBoolean* tk = 
                new tokenBoolean(left->line,left->column,(char*)"true");
            AST_boolean* sol = new AST_boolean(tk);
            
            if ( typeid(*left) == typeid(AST_int) ){
                sol->value = ((AST_int*)left)->value 
                                            >= ((AST_int*)right)->value;
            } else if ( typeid(*left) == typeid(AST_float) ){
                sol->value = ((AST_float*)left)->value 
                                            >= ((AST_float*)right)->value;
            } else if ( typeid(*left) == typeid(AST_char) ){
                sol->value = ((AST_char*)left)->value 
                                            >= ((AST_char*)right)->value;
            } else {
                delete sol;
                return this;
            }
            
            return sol;
            }
            
            break;
    }
    
    return this;
}

AST_expression* AST_un_op::constant_folding(){
    
    if ( typeid(*expr) == typeid(AST_boolean) && oper_type == NOT){
        tokenBoolean* tk = 
                new tokenBoolean(expr->line,expr->column,(char*)"true");
        AST_boolean* sol = new AST_boolean(tk);
        
        sol->value = !((AST_boolean*)expr)->value;
        return sol;
        
    } else if ( typeid(*expr) == typeid(AST_int) && oper_type == NEG){
        tokenInt* tk = 
                new tokenInt(expr->line,expr->column,(char*)"0");
        AST_int* sol = new AST_int(tk);
        
        sol->value = -((AST_int*)expr)->value;
        return sol;
    } else if ( typeid(*expr) == typeid(AST_float) && oper_type == NEG){
        tokenFloat* tk = 
                new tokenFloat(expr->line,expr->column,(char*)"0.0");
        AST_float* sol = new AST_float(tk);
        
        sol->value = -((AST_float*)expr)->value;
        return sol;
    }
    return this;
}

//TODO verificar si es una constante con inicializacion simple
AST_expression* AST_ident::constant_folding(){
    return this;
}

AST_expression* AST_int::constant_folding(){
    return this;
}

AST_expression* AST_float::constant_folding(){
    return this;
}

AST_expression* AST_char::constant_folding(){
    return this;
}

AST_expression* AST_string::constant_folding(){
    return this;
}

AST_expression* AST_boolean::constant_folding(){
    return this;
}

AST_expression* AST_expression::constant_folding(){
    return this;
}

AST_expression* AST_function_call::constant_folding(){
    return this;
}
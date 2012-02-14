
/* Tipo esperado de retorno, util para verificar la validez de los return en
 * el cuerpos de las funciones. Valido como manejo global dado que no se
 * permiten funciones anidadas
 */
symbol::TYPE expected_return = symbol::UNDEFINED;

/* Clase ficticia en la jerarquia, solo necesaria por semantica. No requiere
 * manejo de tabla de simbolos
 */
void AST_node::fill_and_check(symbol_table* st){
    // Empty check
}

/* Clase ficticia en la jerarquia, solo necesaria por semantica. No requiere
 * manejo de tabla de simbolos
 */
void AST_statement::fill_and_check(symbol_table* st){
    // Empty check
}

/* Clase ficticia en la jerarquia, solo necesaria por semantica. No requiere
 * manejo de tabla de simbolos
 */
void AST_expression::fill_and_check(symbol_table* st){
    // Empty check
}

/* Verificacion de ambos operandos de un operador binario
 */
void AST_op::fill_and_check(symbol_table* st){
    left->fill_and_check(st);
    right->fill_and_check(st);
    
    // Ambos argumentos son invalidos
    if ( left->type == symbol::INVALID && right->type == symbol::INVALID ){
        type = symbol::INVALID;
        logger->error(line, column, "Operación de tipos indeterminados");
        return;
    }
    
    /* Un solo argumento invalido, tratar de validar el otro y colocar el valor
     * de la expresion segun el valor de ese argumento
     */
    else if ( left->type == symbol::INVALID || right->type == symbol::INVALID ){
        AST_expression* valido;
        char* nombre_valido;
        
        if ( left->type != symbol::INVALID ){
            valido = left;
            nombre_valido = (char*)"izquierdo";
        } else {
            valido = right;
            nombre_valido = (char*)"derecho";
        }
        
        switch( oper_type ){
            
            /* Operadores aritmeticos: suma, resta, multiplicacion y division */
            case PLUS:
            case MINUS:
            case PROD:
            case DIV:
                
                if (    valido->type == symbol::INT 
                     || valido->type == symbol::FLOAT
                   )
                {
                    type = valido->type;
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN, "Operando '%s' de tipo inválido, esperado float o int.", nombre_valido);
                    logger->error(line, column, e);
                }
                break;
            
            /* Operadores logicos: y, o, implica, consecuencia */
            case AND:
            case OR:
            case IMP:
            case CONSEQ:
                
                if ( valido->type == symbol::BOOLEAN ){
                    type = symbol::BOOLEAN;
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN, "Operando %s de tipo inválido, esperado boolean.", nombre_valido);
                    logger->error(line, column, e);
                }
                break;
            
            /* Operadores de comparación de igualdad: ==, != */
            case EQ:
            case UNEQ:
            
                type = symbol::BOOLEAN;
                break;
            
            /* Operadores de relación de orden: <, <=, >, >= */
            case LESS:
            case LESS_EQ:
            case GREAT:
            case GREAT_EQ:
            
            if (    valido->type == symbol::INT
                 || valido->type == symbol::FLOAT
                 || valido->type == symbol::CHAR
               )
            {
                type = symbol::BOOLEAN;
            }
            break;
        }
        return;
    }
    
    /* else */
    
    // Ambos argumentos validos    
    switch( oper_type ){
        
        /* Operadores aritmeticos: suma, resta, multiplicacion y division */
        case PLUS:
        case MINUS:
        case PROD:
        case DIV:
            
            if ( left->type == right->type ) {
                if ( left->type  == symbol::INT || left->type == symbol::FLOAT )
                {
                    type = left->type;
                } else {
                    // Error de ambos argumentos con tipos invalidos
                    logger->error(line, column, "Operandos de tipo inválido, esperado float o int.");
                    type = symbol::INVALID;
                }
                
            } else {
                // Argumento derecho con tipo invalido
                logger->error(line, column, "Argumentos de tipos incompatibles (esperando float o int).");
                type = symbol::INVALID;
            }
            break;
        
        /* Operadores logicos: y, o, implica, consecuencia */
        case AND:
        case OR:
        case IMP:
        case CONSEQ:
            
            if ( left->type == right->type )
            {
                if ( left->type  == symbol::BOOLEAN )
                {
                    type = symbol::BOOLEAN;
                } else {
                    // Error de ambos argumentos con tipos invalidos
                    logger->error(line, column, "Operandos de tipo inválido, esperado boolean.");
                    type = symbol::INVALID;
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                logger->error(line, column, "Argumentos de tipos incompatibles (esperando boolean).");
                type = symbol::INVALID;
            }
            
            break;
        
        /* Operadores de comparación de igualdad: ==, != */
        case EQ:
        case UNEQ:
            
            if ( left->type == right->type )
            {
                type = symbol::BOOLEAN;
            } else {
                // Error de ambos argumentos con tipos invalidos
                logger->error(line, column, "Operandos de tipos incompatibles");
                type = symbol::INVALID;
            }
            
            break;
        
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
        case LESS_EQ:
        case GREAT:
        case GREAT_EQ:
            
            if ( left->type == right->type )
            {
                if (    left->type  == symbol::INT
                     || left->type == symbol::FLOAT
                     || left->type == symbol::CHAR
                   )
                {
                    type = symbol::BOOLEAN;
                } else {
                     // Error de ambos argumentos con tipos invalidos
                    logger->error(line, column, "Operandos de tipo inválido, esperado float, int o char.");
                    type = symbol::INVALID;
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                logger->error(line, column, "Operandos de tipos incompatibles.");
                type = symbol::INVALID;
            }
            
            break;
        
        default:
            break;
    }
}

/* Verificacion de operando de un operador unario
 */
void AST_un_op::fill_and_check(symbol_table* st){
    expr->fill_and_check(st);
    
    if ( expr->type == symbol::BOOLEAN ){
        if ( oper_type == NOT ){
            type = symbol::BOOLEAN;
        } else {
            // Reportar error de argumento booleano y operando menos unario
            logger->error(line, column, "Operando de tipo boolean, esperado float o int.");
        }
    } else if ( expr->type == symbol::INT || expr->type == symbol::FLOAT ){
        
        if ( oper_type == NEG ){
            type = expr->type;
        } else {
            // Reportar error de argumento numero y operando negacion
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Operando de tipo %s, esperado boolean.",
                expr->type == symbol::INT ? "int": "float");
            logger->error(line, column, e);
        }
    } else {
        // Reportar error de tipo de argumento
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Operando de tipo inválido, esperado %s.",
            oper_type == NOT ? "boolean": "float o int");
        logger->error(line, column, e);
    }
}

void AST_int::fill_and_check(symbol_table* st){
    type = symbol::INT;
}

void AST_float::fill_and_check(symbol_table* st){
    type = symbol::FLOAT;
}

void AST_char::fill_and_check(symbol_table* st){
    type = symbol::CHAR;
}

void AST_boolean::fill_and_check(symbol_table* st){
    type = symbol::BOOLEAN;
}

void AST_ident::fill_and_check(symbol_table* st){
    sym = st->lookup(value);
    
    /* El simbolo existe en la tabla */
    if ( sym ){
        
        /* El simbolo es una function, uso incorrecto como variable */
        if ( sym->is_function ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Identificador '%s' definido como función en %d:%d "
                " (se esperaba variable).", sym->getName().c_str(),
                sym->getLine(), sym->getColumn());
            logger->error(line, column, e);
            sym = 0;
        } else {
            type = sym->getType();
        }
    } else {
        /* El simbolo no existe en la tabla */
        type = symbol::INVALID;

        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Identificador %s no definido previamente.", value.c_str());
        logger->error(line, column, e);
    }
}

void AST_block::fill_and_check(symbol_table* st){
    
    /* Verificar cada statement del bloque, se asume que la tabla de
     * simbolos es nueva o que tiene los argumentos de la funcion en caso
     * de ser el cuerpo de una
     */
    vector<AST_statement*>::iterator it;
    
    for (int i=statements.size()-1; i>=0; i--){
        if ( typeid(*statements[i]) == typeid(AST_block) ){
            symbol_table* nested_block = st->new_son();
            statements[i]->fill_and_check(nested_block);
            delete nested_block;
        } else {
            statements[i]->fill_and_check(st);
        }
    }
    
}

void AST_parameters_list::fill_and_check(symbol_table* st){
    
    /* Verifica el tipo de cada uno de los parametros reales de la llamada de
     * una funcion
     */
    vector<AST_expression*>::iterator it;
    for (it=elem.begin(); it != elem.end(); ++it){
        (*it)->fill_and_check(st);
    }
}

void AST_function_call::fill_and_check(symbol_table* st){
    
    params->fill_and_check(st);
    
    sym = st->lookup(name);
    
    if ( !sym ){

        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Función %s no definida.", name.c_str());
        logger->error(line, column, e);
    } else if ( !sym->is_function ){
        sym = 0;

        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Identificador %s no es un nombre de función.", name.c_str());
        logger->error(line, column, e);
    } else {
        symbol_function* f = (symbol_function*)sym;
        
        if ( params->elem.size() != f->params.size() ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Se esperaban %d argumentos para la función %s, %d recibidos.",
                (int)f->params.size(), name.c_str(), (int)params->elem.size());
            logger->error(line, column, e);
        } else {
            uint nsize = params->elem.size();
            for (uint i=0; i<nsize; i++){
                if ( params->elem[i]->type != symbol::INVALID
                     && params->elem[i]->type != f->params[i] )
                {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN, "Argumento %d de '%s' de tipo inválido.", i, name.c_str());
                    logger->error(line, column, e);
                }
            }
        }
    }
}

void AST_declaration::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_variable_declaration::fill_and_check(symbol_table* st){
    
    int level;
    symbol* previous = st->lookup(sym->getName(), &level);
    
    if ( value ){
        value->fill_and_check(st);
    }
    
    if ( previous ){
        if ( previous->is_function || level == 0 ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Identificador '%s' definido como función en línea %d, columna %d.",
                sym->getName().c_str(), previous->getLine(), previous->getColumn());
            logger->error(sym->getLine(), sym->getColumn(), e);
        } else {
            st->insert(sym);
        }
    } else {
        st->insert(sym);
    }
    
    if (    value 
         && value->type != sym->getType()
         && value->type != symbol::INVALID
       )
    {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Inicialización de '%s' con tipo inválido.", sym->getName().c_str());
        logger->error(line, column, e);
    }
}

void AST_arg_list::fill_and_check(symbol_table* st){
    vector<symbol*>::iterator it;
    
    for ( it=args.begin(); it != args.end(); ++it){
        
        int level = 0;
        symbol* prev = st->lookup( (*it)->getName(), &level );
        if ( prev ){
            if ( level != 0 ){
                st->insert( (*it) );
            } else {
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN, "Argumento '%s' con identificador repetido.", (*it)->getName().c_str());
                logger->error(line, column, e);

                break;
            }
        } else {
            st->insert( (*it) );
        }
    }
}

void AST_function::fill_and_check(symbol_table* st){
    
    symbol_table* nested_block = st->new_son();
    
    expected_return = func->getType();
    
    formal_parameters->fill_and_check(nested_block);
    instructions->fill_and_check(nested_block);
    
    expected_return = symbol::UNDEFINED;
    
    delete nested_block;
}

void AST_program::fill_and_check(symbol_table* st){
    vector<AST_declaration*>::iterator it;
    
    uint nsize = declarations.size();
    for ( uint i = 0; i != nsize; i++){
        if ( typeid(*declarations[i]) == typeid(AST_function) ){
            
            AST_function* f = (AST_function*)declarations[i];
            symbol_function* sf = (symbol_function*)st->lookup( f->func->getName() );
            
            if ( sf ){
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN, "Función '%s' con nombre repetido (primera declaración en %d:%d).",
                    f->func->getName().c_str(), sf->getLine(), sf->getColumn());
                logger->error(f->func->getLine(), f->func->getColumn(), e);
            } else {
                st->insert( f->func );
            }
        }
    }
    
    for ( it = declarations.begin(); it != declarations.end(); it++){
        (*it)->fill_and_check(st);
    }
}

void AST_assignment::fill_and_check(symbol_table* st){
    
    sym = st->lookup(variable);
    
    expr->fill_and_check(st);
    
    if ( !sym ){
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Identificador %s no definido previamente.", variable.c_str());
        logger->error(line, column, e);
    } else if ( sym->isConst() ){
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Intento de asignación a constante '%s'.", variable.c_str());
        logger->error(line, column, e);
    } else if ( expr->type != sym->getType() && expr->type != symbol::INVALID ){
        logger->error(line, column, "Lado derecho e izquierdo de asignación de distintos tipos.");
    }
}

void AST_return::fill_and_check(symbol_table* st){
    
    if ( expected_return == symbol::NONE && expr ){
        logger->error(line, column, "No se esperaba retorno con valor.");
    } else if ( expected_return != symbol::NONE && !expr ){
        logger->error(line, column, "Se esperaba valor de retorno.");
    } else if ( expected_return != symbol::NONE && expr ){
        expr->fill_and_check(st);
        if ( expr->type != expected_return ){
            logger->error(line, column, "Tipo de valor de retorno inválido.");
        }
    }
}

void AST_conditional::fill_and_check(symbol_table* st){
    
    if ( expr ){
        expr->fill_and_check(st);
        
        if ( expr->type != symbol::BOOLEAN ){
            logger->error(expr->line, expr->column, "Se esperaba boolean.");
        }
    }
    
    symbol_table* nested_block = st->new_son();
    block->fill_and_check(nested_block);
    
    delete nested_block;
    
    if ( else_if ){
        else_if->fill_and_check(st);
    }
}

void AST_loop::fill_and_check(symbol_table* st){
    expr->fill_and_check(st);
    if ( expr->type != symbol::INVALID  && expr->type != symbol::BOOLEAN ){
        logger->error(line, column, "Error en condición, se esperaba boolean");
    }
    
    symbol_table* nested_block = st->new_son();
    block->fill_and_check(nested_block);
    delete nested_block;
}

void AST_bounded_loop::fill_and_check(symbol_table* st){

    left_bound->fill_and_check(st);
    right_bound->fill_and_check(st);
    
    if (   left_bound->type == symbol::INVALID 
        || right_bound->type == symbol::INVALID )
    {
        return;
    } else if ( left_bound->type != right_bound->type ){
        logger->error(line, column, "Ambos límites deben tener el mismo tipo.");
    } else if (    left_bound->type != symbol::INT 
                && left_bound->type != symbol::CHAR
                && left_bound->type != symbol::BOOLEAN
              )
    {
        logger->error(line, column, "Se esperaba un tipo discreto int, char, boolean.");
    }
    
    symbol_table* nested_block = st->new_son();
    
    sym = new symbol(name, true, left_bound->type, line_name, column_name,true);
    nested_block->insert(sym);
    
    block->fill_and_check(nested_block);
    delete nested_block;
    
}

void AST_break::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_continue::fill_and_check(symbol_table* st){
    // Empty check
}

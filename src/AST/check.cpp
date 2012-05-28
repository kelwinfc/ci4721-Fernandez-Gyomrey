/* 
 * Verifica para cada tipo de nodo del AST que los tipos sean consistentes,
 * llena la tabla de simbolos.
 * 
 */

/* Tipo esperado de retorno, util para verificar la validez de los return en
* el cuerpos de las funciones. Valido como manejo global dado que no se
* permiten funciones anidadas
*/
TYPE expected_return = UNDEFINED;

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

/* Clase ficticia en la jerarquia, solo necesaria por semantica. No requiere
* manejo de tabla de simbolos
*/
void AST_lval::fill_and_check(symbol_table* st){
    fill_and_check(st, false);
}

void AST_lval::fill_and_check(symbol_table* st, bool lval){
    // Empty check
}

/* Verificacion de ambos operandos de un operador binario
*/
void AST_op::fill_and_check(symbol_table* st){
    if ( left )
        left->fill_and_check(st);
    
    if ( right )
        right->fill_and_check(st);
    
    if ( !left || !right ){
        return;
    }
    
    // Ambos argumentos son invalidos
    if ( left->type == INVALID && right->type == INVALID ){
        type = INVALID;
        logger->error(line, column);
        return;
    }
    
    /* Un solo argumento invalido, tratar de validar el otro y colocar el valor
    * de la expresion segun el valor de ese argumento
    */
    else if ( left->type == INVALID || right->type == INVALID ){
        AST_expression* valido;
        char* nombre_valido;
        
        if ( left->type != INVALID ){
            valido = left;
            nombre_valido = (char*)"izquierdo";
        } else {
            valido = right;
            nombre_valido = (char*)"derecho";
        }
        
        switch( oper_type ){
            
            case MOD:
                
                type = INT;
                
                if ( valido->type != INT ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operando '%s' de tipo %s, esperado int.",
                             nombre_valido,
                             types.types[valido->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                }
                break;
                
            /* Operadores aritmeticos: suma, resta, multiplicacion y division */
            case PLUS:
            case MINUS:
            case PROD:
            case DIV:
                
                if ( valido->type == INT
                     || valido->type == FLOAT
                   )
                {
                    type = valido->type;
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operando '%s' de tipo %s, esperado float o int.",
                             nombre_valido,
                             types.types[valido->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                }
                break;
            
            /* Operadores logicos: y, o, implica, consecuencia */
            case AND:
            case OR:
            case IMP:
            case CONSEQ:
                
                if ( valido->type == BOOLEAN ){
                    type = BOOLEAN;
                } else {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operando %s de tipo %s, esperado boolean.",
                             nombre_valido,
                             types.types[valido->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                }
                break;
            
            /* Operadores de comparación de igualdad: ==, != */
            case EQ:
            case UNEQ:
            
                type = BOOLEAN;
                break;
            
            /* Operadores de relación de orden: <, <=, >, >= */
            case LESS:
            case LESS_EQ:
            case GREAT:
            case GREAT_EQ:
            
                type = BOOLEAN;
                if (    valido->type != INT
                     && valido->type != FLOAT
                     && valido->type != CHAR
                   )
                {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operando %s de tipo %s, esperado int, float "
                             "o char.",
                             nombre_valido,
                             types.types[valido->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                }
                break;
        }
        return;
    }
    
    /* else */
    
    // Ambos argumentos validos
    switch( oper_type ){
        
        case MOD:
            type = INT;
            
            char e[llog::ERR_LEN];
            
            if ( left->type != INT ){
                snprintf(e, llog::ERR_LEN, "Operando de tipo %s, esperado int.",
                         types.types[left->type]->name.c_str());
                logger->error(line, column, e);
            } else if ( right->type != INT ) {
                snprintf(e, llog::ERR_LEN, "Operando de tipo %s, esperado int.",
                         types.types[right->type]->name.c_str());
                logger->error(line, column, e);
            }
            
            break;
            
        /* Operadores aritmeticos: suma, resta, multiplicacion y division */
        case PLUS:
        case MINUS:
        case PROD:
        case DIV:
            
            if ( left->type == right->type ) {
                if ( left->type == INT || left->type == FLOAT )
                {
                    type = left->type;
                } else {
                    // Error de ambos argumentos con tipos invalidos
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operandos de tipo %s, esperado float o int.",
                            types.types[left->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                    
                    type = INVALID;
                }
                
            } else {
                // Argumento derecho con tipo invalido
                char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operandos de tipos incompatibles %s y %s.",
                            types.types[left->type]->name.c_str(),
                            types.types[right->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                type = INVALID;
            }
            break;
        
        /* Operadores logicos: y, o, implica, consecuencia */
        case AND:
        case OR:
        case IMP:
        case CONSEQ:
            
            if ( left->type == right->type )
            {
                type = BOOLEAN;
                if ( left->type != BOOLEAN )
                {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Operandos de tipo %s, esperado boolean.",
                             types.types[left->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                    
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Operandos de tipos %s y %s, esperados boolean.",
                         types.types[left->type]->name.c_str(),
                         types.types[right->type]->name.c_str()
                        );
                logger->error(line, column, e);
            }
            
            break;
        
        /* Operadores de comparación de igualdad: ==, != */
        case EQ:
        case UNEQ:
            
            type = BOOLEAN;
            if ( left->type != right->type )
            {
                // Error de ambos argumentos con tipos invalidos
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Operandos de tipos incompatibles %s y %s.",
                         types.types[left->type]->name.c_str(),
                         types.types[right->type]->name.c_str()
                        );
                logger->error(line, column, e);
            }
            
            break;
        
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
        case LESS_EQ:
        case GREAT:
        case GREAT_EQ:
            
            type = BOOLEAN;
            
            if ( left->type == right->type )
            {
                if (    left->type != INT
                     && left->type != FLOAT
                     && left->type != CHAR
                   )
                {
                    // Error de ambos argumentos con tipos invalidos
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                         "Operandos de tipo %s, esperado float, int o char.",
                         types.types[left->type]->name.c_str()
                        );
                    logger->error(line, column, e);
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Operandos de tipos incompatibles %s y %s",
                         types.types[left->type]->name.c_str(),
                         types.types[right->type]->name.c_str()
                        );
                logger->error(line, column, e);
            }
            
            break;
    }
}

/* Verificacion de operando de un operador unario
*/
void AST_un_op::fill_and_check(symbol_table* st){
    expr->fill_and_check(st);
    
    if ( expr->type == BOOLEAN ){
        if ( oper_type == NOT ){
            type = BOOLEAN;
        } else {
            // Reportar error de argumento booleano y operando menos unario
            logger->error(line, column,
                          "Operando de tipo boolean, esperado float o int.");
        }
    } else if ( expr->type == INT || expr->type == FLOAT ){
        
        if ( oper_type == NEG ){
            type = expr->type;
        } else {
            // Reportar error de argumento numero y operando negacion
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Operando de tipo %s, esperado boolean.",
                expr->type == INT ? "int": "float");
            logger->error(line, column, e);
        }
    } else {
        // Reportar error de tipo de argumento
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Operando de tipo %s, esperado %s.",
                 types.types[expr->type]->name.c_str(),
                 oper_type == NOT ? "boolean": "float o int");
        logger->error(line, column, e);
    }
}

void AST_int::fill_and_check(symbol_table* st){

}

void AST_float::fill_and_check(symbol_table* st){

}

void AST_char::fill_and_check(symbol_table* st){

}

void AST_string::fill_and_check(symbol_table* st){
    offset = strings.push_back(value);
}

void AST_enum_constant::fill_and_check(symbol_table* st){
    for (vector< type_descriptor* >::iterator it = types.types.begin(); it != types.types.end(); ++ it) {
        if (typeid(**it) != typeid(enum_type)) {
            continue;
        }
        set<string> *e = ((enum_type*)(*it))->values;
        enum_index = 0;
        set<string>::iterator ite;
        for (ite = e->begin(); ite != e->end(); ++ ite) {
            if (!value.compare(*ite)) {
                break;
            }
            ++ enum_index;
        }
        if (ite != e->end()) {
            break;
        }
    }
}

void AST_boolean::fill_and_check(symbol_table* st){

}

void AST_ident::fill_and_check(symbol_table* st, bool lval){
    sym = st->lookup(value);
    
    /* El simbolo existe en la tabla */
    if ( sym ){
        
        /* El simbolo es una function, uso incorrecto como variable */
        if ( sym->is_function ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                     "Identificador '%s' definido como función en %d:%d "
                     " (se esperaba variable).", sym->getName().c_str(),
                     sym->getLine(), sym->getColumn());
            logger->error(line, column, e);
            sym = 0;
        }
        /* Intento de acceso a constante como lvalue */
        else if ( lval && sym->isConst() ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                     "Intento de acceso a constante %s definida en %d:%d",
                     sym->getName().c_str(),
                     sym->getLine(), sym->getColumn());
            logger->error(line, column, e);
            
            type = sym->getType();
        } else {
            type = sym->getType();
            
            if ( !lval ){
                sym->unused = false;
            }
        }
    } else {
        /* El simbolo no existe en la tabla */
        type = INVALID;

        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN,
                 "Identificador '%s' no definido previamente.", value.c_str());
        logger->error(line, column, e);
    }
    
}

void AST_ident::check_call(symbol_table* st){ 
    fill_and_check(st);
    
    sym = st->lookup(value);
    
    /* El simbolo existe en la tabla */
    if ( sym && sym->isConst() ){
        if (    typeid(*types.types[sym->getType()]) == typeid(array_descriptor)
             || typeid(*types.types[sym->getType()]) == typeid(struct_type)
             || typeid(*types.types[sym->getType()]) == typeid(union_type)
        )
        {
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                     "Pasaje por referencia de constante %s definida en %d:%d.",
                     sym->getName().c_str(),
                     sym->getLine(), sym->getColumn()
                    );
            logger->error(line, column, e);
        }
    }
}

void AST_dereference::fill_and_check(symbol_table* st, bool lval){
    value->fill_and_check(st, lval);
    
    type_descriptor* td = types.types[value->type];
    
    if ( typeid(*td) == typeid(pointer_descriptor) ){
        type = ((pointer_descriptor*)td)->base;
    } else {
        type = INVALID;
    }
}

void AST_address::fill_and_check(symbol_table* st, bool lval){

    value->fill_and_check(st, true);

    if (typeid(value) == typeid(AST_address)) {
        type = INVALID;
    } else {

        pointer_descriptor* pd = new pointer_descriptor( value->type,
                                                         types.types[
                                                             value->type]->name);

        if ( !types.has_type( pd->name ) ){
            type = types.add_type( pd );
        } else {
            type = types.index_of( pd->name );
            delete pd;
        }
    }
}

void AST_array_access::fill_and_check(symbol_table* st, bool lval){
    
    //en caso de haber un error de sintáxis con recuperación de errores puede no haber valor
    if ( !value )
        return;
    
    value->fill_and_check(st, lval);
    
    if ( typeid(*(types.types[ value->type ])) != typeid(array_descriptor) ){
        char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Intento de acceso indexado a tipo %s.",
                     types.types[value->type]->name.c_str());
            logger->error(line, column, e);
        type = INVALID;
    } else {
        type = ((array_descriptor*)types.types[ value->type ])->base;
    }
    
}

void AST_struct_access::fill_and_check(symbol_table* st, bool lval){
    value->fill_and_check(st, lval);
    
    type_descriptor* td = types.types[value->type];
    
    if ( value->type == INVALID ){
        type = INVALID;
        return;
    }
    
    if (    typeid(*td) != typeid(struct_type) 
         && typeid(*td) != typeid(union_type) 
    )
    {
        char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Intento de acceso a campo en tipo %s.",
                     types.types[value->type]->name.c_str());
            logger->error(line, column, e);
        type = INVALID;
    } else {
        symbol* sym = ((struct_type*)td)->fields->lookup( field );
        
        if ( sym != 0 ){
            type = sym->getType();
            
            if ( sym->isConst() ){
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Intento de acceso a constante %s definida en %d:%d",
                         sym->getName().c_str(),
                         sym->getLine(), sym->getColumn()
                    );
                logger->error(line, column, e);
            }
        } else {
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Tipo %s no posee campo %s",
                     types.types[value->type]->name.c_str(),
                     field.c_str()
                    );
            logger->error(line, column, e);
            type = INVALID;
        }
        
    }
}

void AST_conversion::fill_and_check(symbol_table* st){
    expr->fill_and_check(st);
    original_type = expr->type;
    if (!types.is_base(original_type) || !types.is_base(type) || STRING == original_type || STRING == type) {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Conversión permitida sólo entre tipos primitivos (%s a %s)",
                 types.types[original_type]->name.c_str() ,types.types[type]->name.c_str());
        logger->error(line, column, e);
        type = INVALID;
    }
}

void AST_block::fill_and_check(symbol_table* st){
    
    /* Verificar cada statement del bloque, se asume que la tabla de
     * simbolos es nueva o que tiene los argumentos de la funcion en caso
     * de ser el cuerpo de una
     */
    vector<AST_statement*>::iterator it;
    
    for (vector<AST_statement*>::iterator it = statements.begin();
         it != statements.end(); ++ it)
     {
        if ( typeid(**it) == typeid(AST_block) ){
            symbol_table* nested_block = st->new_son();
            (*it)->fill_and_check(nested_block);
            
            delete nested_block;
        } else {
            (*it)->fill_and_check(st);
        }
    }
}

void AST_parameters_list::fill_and_check(symbol_table* st,
                                         vector<bool>& constant)
{
    /* Verifica el tipo de cada uno de los parametros reales de la llamada de
     * una funcion
     */
    for (vector<AST_expression*>::iterator it=elem.begin(); it != elem.end(); ++it){
        if (0 != *it) {
            (*it)->fill_and_check(st);
            (*it)->check_call(st);
        }
    }
}

void AST_parameters_list::fill_and_check(symbol_table* st){
    
    /* Verifica el tipo de cada uno de los parametros reales de la llamada de
     * una funcion
     */
    for (vector<AST_expression*>::iterator it=elem.begin(); it != elem.end(); ++it){
        if (0 != *it) {
            (*it)->fill_and_check(st);
            (*it)->check_call(st);
        }
    }
}

void AST_function_call::fill_and_check(symbol_table* st){
    
    params->fill_and_check(st);
    
    sym = st->lookup(name);
    
    if ( !sym ){

        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Función '%s' no definida.", name.c_str());
        logger->error(line, column, e);
        
        type = UNDEFINED;
    } else if ( !sym->is_function ){
        sym = 0;

        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Identificador '%s' no es un nombre de función.", name.c_str());
        logger->error(line, column, e);
        
        type = UNDEFINED;
    } else {
        symbol_function* f = (symbol_function*)sym;
        
        if ( params->elem.size() != f->params.size() ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Se esperaban %d argumentos para la función '%s', %d recibidos.",
                (int)f->params.size(), name.c_str(), (int)params->elem.size());
            logger->error(line, column, e);
        } else {
            uint nsize = params->elem.size();
            
            for (uint i=0; i<nsize; i++){
                if ( params->elem[i]->type != INVALID
                     && params->elem[i]->type != f->params[i] )
                {
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Argumento %d de '%s' de tipo inválido %s.",
                             i, name.c_str(),
                             types.types[params->elem[i]->type]->name.c_str()
                            );
                    logger->error(line, column, e);
                }
            }
        }
        
        type = f->getType();
    }
}

void AST_declaration::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_variable_declaration::fill_and_check(symbol_table* st){

    if (types.has_type(sym->getName())) {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Identificador '%s' ya existe como tipo '%s'.",
            sym->getName().c_str(), types.get_type(sym->getName())->name.c_str());
        logger->error(sym->getLine(), sym->getColumn(), e);
    }

    sym->offset    = st->accumulated_offset;
    sym->width     = types.types[sym->getType()]->width;
    sym->alignment = types.types[sym->getType()]->alignment;
    
    int level;
    symbol* previous = st->lookup(sym->getName(), &level);
    
    if ( value ){
        value->fill_and_check(st);
    }
    
    if ( previous ){
        if ( previous->is_function ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Identificador '%s' ya fue definido previamente como función en línea %d, columna %d.",
                sym->getName().c_str(), previous->getLine(), previous->getColumn());
            logger->error(sym->getLine(), sym->getColumn(), e);
        } else if ( level == 0 ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                     "Identificador '%s' ya fue definido previamente en "
                     "línea %d, columna %d.",
                sym->getName().c_str(), previous->getLine(), previous->getColumn());
            logger->error(sym->getLine(), sym->getColumn(), e);
        } else {
            st->insert(sym);
        }
    } else {
        st->insert(sym);
    }
    
    if ( value
         && value->type != sym->getType()
         && value->type != INVALID
         && sym->getType() != INVALID
       )
    {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN,
                 "Inicialización de '%s' con tipo inválido %s, esperado %s.",
                 sym->getName().c_str(),
                 types.types[value->type]->name.c_str(),
                 types.types[sym->getType()]->name.c_str()
                );
        logger->error(line, column, e);
    }
}

void AST_arg_list::fill_and_check(symbol_table* st){
    
    for ( vector<symbol*>::iterator it =args.begin(); it != args.end(); ++it){
        
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

void AST_discrete_arg_list::fill_and_check(symbol_table* st){
    
    vector< pair<int, int> >::iterator it_boundaries = boundaries.begin();
    
    for ( vector<symbol*>::iterator it =args.begin(); it != args.end(); ++it){
        
        bool verify_types = true;
        
        int level = 0;
        symbol* prev = st->lookup( (*it)->getName(), &level );
        if ( prev ){
            if ( level != 0 ){
                st->insert( (*it) );
                prev = *it;
            } else {
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Argumento '%s' con identificador repetido.",
                         (*it)->getName().c_str());
                logger->error(line, column, e);

                verify_types = false;
                break;
            }
        } else {
            st->insert( (*it) );
            prev = *it;
        }
        
        if ( verify_types ){
            pair<int,int> b = *it_boundaries;
            if ( b.first > b.second ){
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN, "Argumento '%s' con rango vacio.",
                         (*it)->getName().c_str());
                logger->error(line, column, e);
            }
        }
        
        ++it_boundaries;
    }
    
}

void AST_function::fill_and_check(symbol_table* st){
    
    symbol_table* nested_block = st->new_son();
    
    expected_return = func->getType();
    formal_parameters->fill_and_check(nested_block);
    instructions->fill_and_check(nested_block);
    
    expected_return = UNDEFINED;
    
    delete nested_block;
    
}

extern int yylineno;

void AST_program::fill_and_check(symbol_table* st){
    
    // Declaracion previa de todas las funciones
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
    
    // Verificacion de existencia de main
    {
        symbol_function* sf = (symbol_function*)st->lookup( "main" );
        
        if ( !sf ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Falta declaración de función main.");
            logger->error(yylineno+1, 0, e);
        } else if ( sf->getType() != NONE ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "main debe tener tipo de retorno none.");
            logger->error(sf->getLine(), sf->getColumn(), e);
        } else if ( sf->params.size() ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "main no debe recibir argumentos.");
            logger->error(sf->getLine(), sf->getColumn(), e);
        }
    }
    
    // Verificacion del programa
    vector<AST_declaration*>::iterator it;
    for ( it = declarations.begin(); it != declarations.end(); it++){
        (*it)->fill_and_check(st);
        
        if ( typeid(**it) == typeid(AST_variable_declaration) ){
            AST_variable_declaration* var = 
                (AST_variable_declaration*)*it;
            
            if (    var->value
                 && var->value->type != INVALID 
                 && var->value->has_functions()
               )
            {
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN,
                         "Inicialización de variable %s involucra llamada a función.",
                         var->sym->getName().c_str());
                logger->error(var->line, var->column, e);
            }
        }
    }
}

void AST_assignment::fill_and_check(symbol_table* st){
    
    lvalue->fill_and_check(st, true);
    if (0 != expr) {
        expr->fill_and_check(st);
    }
    
    if ( 0 != expr && expr->type != lvalue->type
                && expr->type != INVALID 
                && lvalue->type != INVALID
                && expr->type != UNDEFINED
                && lvalue->type != UNDEFINED
              )
    {
        
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN,
                 "Intento de asignación de tipo %s a variable de tipo %s.",
                 types.types[expr->type]->name.c_str(),
                 types.types[lvalue->type]->name.c_str()
                );
        logger->error(line, column, e);
    }
}

void AST_return::fill_and_check(symbol_table* st){
    
    if ( expected_return == NONE && expr ){
        logger->error(line, column, "No se esperaba retorno con valor.");
    } else if ( expected_return != NONE && !expr ){
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN,
                 "Se esperaba retorno de tipo %s.",
                 types.types[expected_return]->name.c_str()
                );
        logger->error(line, column, e);
    } else if ( expected_return != NONE && expr ){
        expr->fill_and_check(st);
        if ( expr->type != expected_return ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN,
                 "Se esperaba retorno de tipo %s, %s obtenido.",
                 types.types[expected_return]->name.c_str(),
                 types.types[expr->type]->name.c_str()
                );
            logger->error(line, column, e);
        }
    }
}

void AST_conditional::fill_and_check(symbol_table* st){
    
    if ( expr ){
        expr->fill_and_check(st);
        
        if ( expr->type != BOOLEAN ){
            logger->error(expr->line, expr->column, "Se esperaba boolean.");
        }
    }
    
    symbol_table* nested_block = st->new_son();
    blck->fill_and_check(nested_block);
    
    delete nested_block;
    
    if ( else_if ){
        else_if->fill_and_check(st);
    }
}

void AST_loop::fill_and_check(symbol_table* st){
    if (0 != expr) {
        expr->fill_and_check(st);
        if ( expr->type != INVALID && expr->type != BOOLEAN ){
            logger->error(line, column, "Se esperaba boolean");
        }
    }
    
    symbol_table* nested_block = st->new_son();
    blck->fill_and_check(nested_block);
    delete nested_block;
}

void AST_bounded_loop::fill_and_check(symbol_table* st){

    if (0 != left_bound) {
        left_bound->fill_and_check(st);
    }
    if (0 != right_bound) {
        right_bound->fill_and_check(st);
    }
    
    if ( 0 == left_bound || left_bound->type == INVALID
        || 0 == right_bound || right_bound->type == INVALID )
    {
        return;
    } else if ( left_bound->type != right_bound->type ){
        logger->error(line, column, "Ambos límites deben tener el mismo tipo.");
    } else if ( left_bound->type != INT
                && left_bound->type != CHAR
                && left_bound->type != BOOLEAN
              )
    {
        logger->error(line, column, "Se esperaba un tipo discreto int, char, boolean.");
    }
    
    symbol_table* nested_block = st->new_son();
    
    sym = new symbol(name, true, left_bound->type, line_name, column_name,true);
    nested_block->insert(sym);
    
    blck->fill_and_check(nested_block);
    delete nested_block;
    
}

void AST_break::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_continue::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_read::fill_and_check(symbol_table* st){
    sym = st->lookup(variable);
    
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
        }
    } else {
        /* El simbolo no existe en la tabla */
        
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, 
                 "Identificador '%s' no definido previamente.",
                 variable.c_str()
                );
        logger->error(line, column, e);
    }
}

void AST_print::fill_and_check(symbol_table* st){
    
    vector<AST_expression*>::iterator it;
    for (it=list.begin(); it!=list.end(); ++it){
        cout << "aca\n";
        (*it)->print(0);
        (*it)->fill_and_check(st);
        cout << "aca no\n";
    }
}

void AST_fill::fill_and_check(symbol_table* st){
    
    expr->fill_and_check(st);
    
    if ( expr->type != INVALID
         && typeid(*types.types[expr->type]) != typeid(array_descriptor))
    {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, "Expresion debe evaluar tipo arreglo.");
        logger->error(line, column, e);
        
        return;
    }
    
    sym = st->lookup(function);
    
    if ( sym ){
        
        if ( !sym->is_function ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Esperada identificador de funcion.");
            logger->error(line, column, e);
            sym = 0;
        } else {
            symbol_function* s = (symbol_function*)sym;
            
            /* Numero de argumentos invalidos */
            if ( s->params.size() == 0 || s->params.size() > 2 ){
                
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN, "Funcion con firma invalida.");
                logger->error(line, column, e);
                
            } else {
                array_descriptor* ad = 
                    (array_descriptor*)types.types[expr->type];
                TYPE base = ad->base;
                
                /* Primer argumento debe ser entero para recorrer el arreglo */
                if ( s->params[0] != INT ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Se espera funcion con primer "
                             "argumento de tipo int, recibido %s",
                             ad->name.c_str()
                            );
                    logger->error(line, column, e);
                }
                
                /* Si solo tiene un argumento el tipo de retorno de la funcion
                 * debe coincidir con el tipo base del arreglo
                 */
                
                if ( s->params.size() == 1 && s->getType() != base ){
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Se espera funcion con retorno de tipo %s, %s%s.",
                             "recibida funcion con retorno ",
                             types.types[base]->name.c_str(),
                             types.types[s->getType()]->name.c_str()
                            );
                    logger->error(line, column, e);
                } 
                /* Tiene dos argumentos, el tipo de retorno debe ser none y el
                 * segundo debe ser o bien el tipo base del arreglo o un
                 * apuntador al tipo base (pase por referencia)
                 */
                else if ( s->params.size() == 2 ){
                    char e[llog::ERR_LEN];
                    if ( s->getType() != NONE ){
                        snprintf(e, llog::ERR_LEN,
                                 "Se espera funcion con retorno vacio");
                    } else if ( s->params[1] == base ){
                        return ;
                    } else if ( typeid(* (types.types[s->params[1]]) )
                                         == typeid(pointer_descriptor)
                                && ((pointer_descriptor*)
                                    (types.types[s->params[1]]))->base 
                                         == base
                               )
                    {
                        return ;
                    } else {
                        
                        snprintf(e, llog::ERR_LEN,
                                "Se esperaba funcion con segundo argumento "
                                "de tipo %s o apuntador a %s.",
                                 types.types[base]->name.c_str(),
                                 types.types[base]->name.c_str()
                                );
                    }
                    logger->error(line, column, e);
                }
            }
        }
    } else {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, 
                 "Identificador '%s' no definido previamente.",
                 function.c_str()
                );
        logger->error(line, column, e);
    }
    
}

void AST_map::fill_and_check(symbol_table* st){
    
    src->fill_and_check(st);
    dst->fill_and_check(st);
    
    if ( src->type != INVALID && dst->type != INVALID )
    {
        if ( typeid(*types.types[src->type]) != typeid(array_descriptor) ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Expresion debe evaluar tipo arreglo.");
            logger->error(src->line, src->column, e);
            
            return;
        } else if (typeid(*types.types[dst->type]) != typeid(array_descriptor)){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Expresion debe evaluar tipo arreglo.");
            logger->error(dst->line, dst->column, e);
            
            return;
        } else if ( types.types[src->type] != types.types[dst->type] ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Ambos arreglos deben ser del mismo tipo.");
            logger->error(line, column, e);
            
            return;
        }
    }
    
    sym = st->lookup(function);
    
    if ( sym ){
        
        if ( !sym->is_function ){
            char e[llog::ERR_LEN];
            snprintf(e, llog::ERR_LEN, "Esperada identificador de funcion.");
            logger->error(line, column, e);
            sym = 0;
        } else {
            symbol_function* s = (symbol_function*)sym;
            
            array_descriptor* ad=(array_descriptor*)types.types[src->type];
            TYPE base = ad->base;
            
            if ( s->params.size() == 0 || s->params.size() > 2 ){
                
                char e[llog::ERR_LEN];
                snprintf(e, llog::ERR_LEN, "Funcion con firma invalida.");
                logger->error(line, column, e);
                
            } else if ( s->params.size() == 1 ){
                if ( s->getType() != base ){
                    
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Se espera funcion con retorno de tipo %s,"
                             "recibida funcion con retorno %s",
                             types.types[base]->name.c_str(),
                             types.types[s->getType()]->name.c_str()
                            );
                    logger->error(line, column, e);
                    
                } else if ( s->params[0] != base ){
                    
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Se espera funcion con primer "
                             "argumento de tipo %s, recibido %s",
                             types.types[base]->name.c_str(),
                             types.types[s->params[0]]->name.c_str()
                            );
                    logger->error(line, column, e);
                    
                }
            } else if ( s->params.size() == 2 ){
                if ( s->getType() != NONE ){
                    
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Se espera funcion con retorno vacio.");
                    logger->error(line, column, e);
                    
                } else if ( s->params[0] != base ){
                    
                    char e[llog::ERR_LEN];
                    snprintf(e, llog::ERR_LEN,
                             "Se espera funcion con primer "
                             "argumento de tipo %s, recibido %s",
                             types.types[base]->name.c_str(),
                             types.types[s->params[0]]->name.c_str()
                            );
                    logger->error(line, column, e);
                    
                } else {
                    if ( types.is_base(base) ){
                        if ( typeid(*(types.types[s->params[1]]))
                             == typeid(pointer_descriptor) )
                        {
                            pointer_descriptor* pd =
                               (pointer_descriptor*)(types.types[s->params[1]]);
                            
                            if ( pd->base != base ){
                                char e[llog::ERR_LEN];
                                snprintf(e, llog::ERR_LEN,
                                     "Se espera referencia a %s,"
                                     "recibida referencia a %s.",
                                     types.types[base]->name.c_str(),
                                     types.types[pd->base]->name.c_str()
                                    );
                                logger->error(line, column, e);
                            }
                        } else {
                            char e[llog::ERR_LEN];
                            snprintf(e, llog::ERR_LEN,
                                 "Se espera referencia a %s",
                                 types.types[base]->name.c_str()
                                );
                            logger->error(line, column, e);
                        }
                    } else {
                        if ( base != s->params[1] ){
                            if ( typeid(*(types.types[s->params[1]]))
                                 == typeid(pointer_descriptor) )
                            {
                                pointer_descriptor* pd =
                                 (pointer_descriptor*)types.types[s->params[1]];
                                
                                if ( pd->base != base ){
                                    char e[llog::ERR_LEN];
                                    snprintf(e, llog::ERR_LEN,
                                         "Se espera referencia a %s, "
                                         "recibida referencia a %s.",
                                         types.types[base]->name.c_str(),
                                         types.types[pd->base]->name.c_str()
                                        );
                                    logger->error(line, column, e);
                                }
                            } else {
                                char e[llog::ERR_LEN];
                                snprintf(e, llog::ERR_LEN,
                                     "Se espera referencia a %s",
                                     types.types[base]->name.c_str()
                                    );
                                logger->error(line, column, e);
                            }
                        }
                    }
                }
            }
        }
    } else {
        char e[llog::ERR_LEN];
        snprintf(e, llog::ERR_LEN, 
                 "Identificador '%s' no definido previamente.",
                 function.c_str()
                );
        logger->error(line, column, e);
    }
    
}
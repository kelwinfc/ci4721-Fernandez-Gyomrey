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
                    logger->buf << "Operando '" << nombre_valido
                                << "' de tipo '" << types.types[valido->type]->name
                                << "', esperando int.";
                    logger->error(line, column);
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
                    logger->buf << "Operando '" << nombre_valido
                                << "' de tipo '" << types.types[valido->type]->name
                                << "', esperando float o int.";
                    logger->error(line, column);
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
                    logger->buf << "Operando '" << nombre_valido
                                << "' de tipo '" << types.types[valido->type]->name
                                << "', esperando boolean.";
                    logger->error(line, column);
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
                    logger->buf << "Operando '" << nombre_valido
                                << "' de tipo '" << types.types[valido->type]->name
                                << "', esperando float o int.";
                    logger->error(line, column);
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
            
            if ( left->type != INT ){
                logger->buf << "Operando izquierdo de tipo '" << types.types[left->type]->name
                            << "', esperando int.";
                logger->error(line, column);
            } else if ( right->type != INT ) {
                logger->buf << "Operando derecho de tipo '" << types.types[right->type]->name
                            << "', esperando int.";
                logger->error(line, column);
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
                    logger->buf << "Operando izquierdo de tipo '" << types.types[left->type]->name
                                << "', esperando float o int.";
                    logger->error(line, column);
                    
                    type = INVALID;
                }
                
            } else {
                // Argumento derecho con tipo invalido
                logger->buf << "Operandos de tipos incompatibles '"
                            << types.types[left->type]->name << "' y '"
                            << types.types[right->type]->name << "'";
                logger->error(line, column);
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
                    logger->buf << "Operandos de tipo '" << types.types[left->type]->name
                                << "', esperando boolean.";
                    logger->error(line, column);
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                logger->buf << "Operandos de tipos '"
                            << types.types[left->type]->name << "' y '"
                            << types.types[right->type]->name << "' esperados boolean.";
                logger->error(line, column);
            }
            
            break;
        
        /* Operadores de comparación de igualdad: ==, != */
        case EQ:
        case UNEQ:
            
            type = BOOLEAN;
            if ( left->type != right->type )
            {
                // Error de ambos argumentos con tipos invalidos
                logger->buf << "Operandos de tipos imcompratibles '"
                            << types.types[left->type]->name << "' y '"
                            << types.types[right->type]->name << "'";
                logger->error(line, column);
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
                    logger->buf << "Operandos de tipos '"
                                << types.types[left->type]->name
                                << "' esperando float, int o char";
                    logger->error(line, column);
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                logger->buf << "Operandos de tipos imcompratibles '"
                            << types.types[left->type]->name << "' y '"
                            << types.types[right->type]->name << "'";
                logger->error(line, column);
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
            logger->buf << "Operando de tipo boolean, esperado float o int.";
            logger->error(line, column);
        }
    } else if ( expr->type == INT || expr->type == FLOAT ){
        
        if ( oper_type == NEG ){
            type = expr->type;
        } else {
            // Reportar error de argumento numero y operando negacion
            logger->buf << "Operando de tipo '" << (expr->type == INT ? "int": "float")
                        << "', esperado boolean.";
            logger->error(line, column);
        }
    } else {
        // Reportar error de tipo de argumento
        logger->buf << "Operando de tipo '" << types.types[expr->type]->name
                    << "', esperado '" << (oper_type == NOT ? "boolean": "float o int")
                    << "'.";
        logger->error(line, column);
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
            logger->buf << "Identificador '" << sym->getName()
                        << "' definido como función en "
                        << sym->getLine() << ':' << sym->getColumn()
                        << " (se esperaba variable).";
            logger->error(line, column);
            sym = 0;
        }
        /* Intento de acceso a constante como lvalue */
        else if ( lval && sym->isConst() ){
            logger->buf << "Intento de acceso a constante '"
                        << sym->getName() << "' definida en "
                        << sym->getLine() << ':' << sym->getColumn();
            logger->error(line, column);
            
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
        logger->buf << "Identificador '" << value << "' no definido previamente.";
        logger->error(line, column);
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
            logger->buf << "Pasaje por referencia de constante '"
                        << sym->getName() << "' definida en "
                        << sym->getLine() << ':' << sym->getColumn();
            logger->error(line, column);
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
        logger->buf << "Intento de acceso indexado a tipo '"
                    << types.types[value->type]->name << "'.";
        logger->error(line, column);
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
        logger->buf << "Intento de acceso a campo en tipo '"
                    << types.types[value->type]->name << "'.";
        logger->error(line, column);
        type = INVALID;
    } else {
        sym = ((struct_type*)td)->fields->lookup( field );
        
        if ( sym != 0 ){
            type = sym->getType();
            
            if ( sym->isConst() ){
                logger->buf << "Intento de acceso a constante '"
                            << sym->getName() << "' definida en "
                            << sym->getLine() << ':' << sym->getColumn();
                logger->error(line, column);
            }
            union_access = typeid(*td) == typeid(struct_type);
        } else {
            logger->buf << "Tipo '" << types.types[value->type]->name
                        << "' no posee campo '" << field << "'.";
            logger->error(line, column);
            type = INVALID;
        }
        
    }
}

void AST_rlval::fill_and_check(symbol_table* st, bool lval){
    value->fill_and_check(st, lval);
    type = value->type;
}

void AST_conversion::fill_and_check(symbol_table* st){
    expr->fill_and_check(st);
    original_type = expr->type;
    if (!types.is_base(original_type) || !types.is_base(type) || STRING == original_type || STRING == type) {
        logger->buf << "Conversión permitida sólo entre tipos primitivos ('"
                    << types.types[original_type]->name << "' a '"
                    << types.types[type]->name << "'.";
        logger->error(line, column);
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

        logger->buf << "Función '" << name << "' no definida.";
        logger->error(line, column);
        
        type = UNDEFINED;
    } else if ( !sym->is_function ){
        sym = 0;

        logger->buf << "Identificador '" << name << "' no es un nombre de función.";
        logger->error(line, column);
        
        type = UNDEFINED;
    } else {
        symbol_function* f = (symbol_function*)sym;
        
        if ( params->elem.size() != f->params.size() ){
            logger->buf << "Se esperaban " << f->params.size()
                        << " argumentos para la función '" << name
                        << "', " << params->elem.size() << " recibidos.";
            logger->error(line, column);
        } else {
            uint nsize = params->elem.size();
            
            for (uint i=0; i<nsize; i++){
                if ( params->elem[i]->type != INVALID
                     && params->elem[i]->type != f->params[i] )
                {
                    logger->buf << "Argumento " << i << " de '" << name << "' de tipo inválido '"
                                << types.types[params->elem[i]->type]->name << "'.";
                    logger->error(line, column);
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
        logger->buf << "Identificador '" << sym->getName()
                    << "' ya existe como tipo '" << types.get_type(sym->getName())->name << "'.";
        logger->error(sym->getLine(), sym->getColumn());
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
            logger->buf << "Identificador '" << sym->getName()
                        << "' ya fue definido previamente como función en "
                        << sym->getLine() << ':' << sym->getColumn();
            logger->error(sym->getLine(), sym->getColumn());
        } else if ( level == 0 ){
            logger->buf << "Identificador '" << sym->getName()
                        << "' ya fue definido previamente en "
                        << sym->getLine() << ':' << sym->getColumn();
            logger->error(sym->getLine(), sym->getColumn());
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
        logger->buf << "Inicialización de '" << sym->getName()
                    << "' con tipo inválido '" << types.types[value->type]->name
                    << "', esperado '" << types.types[sym->getType()]->name << "'.";
        logger->error(line, column);
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
                logger->buf << "Argumento '" << (*it)->getName() << "' con identificador repetido.";
                logger->error(line, column);

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
                logger->buf << "Argumento '" << (*it)->getName() << "' con identificador repetido.";
                logger->error(line, column);

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
                logger->buf << "Argumento '" << (*it)->getName() << "' con rango vacio.";
                logger->error(line, column);
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

void AST::fill_and_check(symbol_table* st){
    
    // Declaracion previa de todas las funciones
    uint nsize = declarations.size();
    for ( uint i = 0; i != nsize; i++){
        if ( typeid(*declarations[i]) == typeid(AST_function) ){
            
            AST_function* f = (AST_function*)declarations[i];
            symbol_function* sf = (symbol_function*)st->lookup( f->func->getName() );
            
            if ( sf ){
                logger->buf << "Función '" << f->func->getName()
                            << "' con nombre repetido (primera declaración en "
                            << sf->getLine() << ':' << sf->getColumn() << ").";
                logger->error(f->func->getLine(), f->func->getColumn());
            } else {
                st->insert( f->func );
            }
        }
    }
    
    // Verificacion de existencia de main
    {
        symbol_function* sf = (symbol_function*)st->lookup( "main" );
        
        if ( !sf ){
            logger->buf << "Falta declaración de función main.";
            logger->error(yylineno+1, 0);
        } else if ( sf->getType() != NONE ){
            logger->buf << "main debe tener tipo de retorno none.";
            logger->error(sf->getLine(), sf->getColumn());
        } else if ( sf->params.size() ){
            logger->buf << "main no debe recibir argumentos.";
            logger->error(sf->getLine(), sf->getColumn());
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
                logger->buf << "Inicialización de variable '" << var->sym->getName()
                            << "' involucra llamada a función.";
                logger->error(var->line, var->column);
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
        logger->buf << "Intento de asignación de tipo '"
                    << types.types[expr->type]->name << "' a variable de tipo '"
                    << types.types[lvalue->type]->name << "'.";
        logger->error(line, column);
    }
}

void AST_procedure_call::fill_and_check(symbol_table* st){
    funcall->fill_and_check(st);
    has_return = funcall->has_return;
}

void AST_return::fill_and_check(symbol_table* st){
    
    if ( expected_return == NONE && expr ){
        logger->buf << "No se esperaba retorno con valor.";
        logger->error(line, column);
    } else if ( expected_return != NONE && !expr ){
        logger->buf << "Se esperaba retorno de tipo '"
                    << types.types[expected_return]->name << "'.";
        logger->error(line, column);
    } else if ( expected_return != NONE && expr ){
        expr->fill_and_check(st);
        if ( expr->type != expected_return ){
            logger->buf << "Se esperaba retorno de tipo '"
                        << types.types[expected_return]->name << "', '"
                        << types.types[expr->type]->name << "' obtenido.";
            logger->error(line, column);
        }
    }
}

void AST_conditional::fill_and_check(symbol_table* st){
    
    if ( expr ){
        expr->fill_and_check(st);
        
        if ( expr->type != BOOLEAN ){
            logger->buf << "Se esperaba boolean.";
            logger->error(expr->line, expr->column);
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
            logger->buf << "Se esperaba boolean.";
            logger->error(line, column);
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
        logger->buf << "Ambos límites deben tener el mismo tipo.";
        logger->error(line, column);
    } else if ( left_bound->type != INT
                && left_bound->type != CHAR
                && left_bound->type != BOOLEAN
              )
    {
        logger->buf << "Se esperaba un tipo discreto int, char, boolean.";
        logger->error(line, column);
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
            logger->buf << "Identificador '" << sym->getName()
                        << "' definido como función en "
                        << sym->getLine() << ':' << sym->getColumn() << '.';
            logger->error(line, column);
            sym = 0;
        }
    } else {
        /* El simbolo no existe en la tabla */
        logger->buf << "Identificador '" << variable << "' no definido previamente.";
        logger->error(line, column);
    }
}

void AST_print::fill_and_check(symbol_table* st){
    
    vector<AST_expression*>::iterator it;
    for (it=list.begin(); it!=list.end(); ++it){
        (*it)->fill_and_check(st);
    }
}

void AST_fill::fill_and_check(symbol_table* st){
    
    expr->fill_and_check(st);
    
    if ( expr->type != INVALID
         && typeid(*types.types[expr->type]) != typeid(array_descriptor))
    {
        logger->buf << "Expresion debe evaluar tipo arreglo.";
        logger->error(line, column);
        
        return;
    }
    
    sym = st->lookup(function);
    
    if ( sym ){
        
        if ( !sym->is_function ){
            logger->buf << "Esperado identificador de función.";
            logger->error(line, column);
            sym = 0;
        } else {
            symbol_function* s = (symbol_function*)sym;
            
            /* Numero de argumentos invalidos */
            if ( s->params.size() == 0 || s->params.size() > 2 ){
                
                logger->buf << "Funcion con firma invalida.";
                logger->error(line, column);
            } else {
                array_descriptor* ad = 
                    (array_descriptor*)types.types[expr->type];
                TYPE base = ad->base;
                
                /* Primer argumento debe ser entero para recorrer el arreglo */
                if ( s->params[0] != INT ){
                    logger->buf << "Se espera funcion con primer "
                                   "argumento de tipo int, recibido "
                                << ad->name << '.';
                    logger->error(line, column);
                }
                
                /* Si solo tiene un argumento el tipo de retorno de la funcion
                 * debe coincidir con el tipo base del arreglo
                 */
                
                if ( s->params.size() == 1 && s->getType() != base ){
                    logger->buf << "Se espera funcion con retorno de tipo '"
                                << types.types[base]->name
                                << "', recibida funcion con retorno '"
                                << types.types[s->getType()]->name << "'.";
                    logger->error(line, column);
                } 
                /* Tiene dos argumentos, el tipo de retorno debe ser none y el
                 * segundo debe ser o bien el tipo base del arreglo o un
                 * apuntador al tipo base (pase por referencia)
                 */
                else if ( s->params.size() == 2 ){
                    if ( s->getType() != NONE ){
                        logger->buf << "Se espera funcion con retorno vacío";
                        logger->error(line, column);
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
                        logger->buf << "Se esperaba funcion con segundo "
                                       "argumento de tipo o apuntador a '"
                                    << types.types[base]->name << "'.";
                        logger->error(line, column);
                    }
                }
            }
        }
    } else {
        logger->buf << "Identificador '" << function << "' no definido previamente.";
        logger->error(line, column);
    }
    
}

void AST_map::fill_and_check(symbol_table* st){
    
    src->fill_and_check(st);
    dst->fill_and_check(st);
    
    if ( src->type != INVALID && dst->type != INVALID )
    {
        if ( typeid(*types.types[src->type]) != typeid(array_descriptor) ){
            logger->buf << "Expresion debe evaluar tipo arreglo.";
            logger->error(src->line, src->column);
            return;
        } else if (typeid(*types.types[dst->type]) != typeid(array_descriptor)){
            logger->buf << "Expresion debe evaluar tipo arreglo.";
            logger->error(dst->line, dst->column);
            return;
        } else if ( types.types[src->type] != types.types[dst->type] ){
            logger->buf << "Ambos arreglos deben ser del mismo tipo.";
            logger->error(line, column);
            return;
        }
    }
    
    sym = st->lookup(function);
    
    if ( sym ){
        
        if ( !sym->is_function ){
            logger->buf << "Esperada identificador de funcion.";
            logger->error(line, column);
            sym = 0;
        } else {
            symbol_function* s = (symbol_function*)sym;
            
            array_descriptor* ad=(array_descriptor*)types.types[src->type];
            TYPE base = ad->base;
            
            if ( s->params.size() == 0 || s->params.size() > 2 ){
                logger->buf << "Funcion con firma invalida.";
                logger->error(line, column);
                
            } else if ( s->params.size() == 1 ){
                if ( s->getType() != base ){
                    logger->buf << "Se espera funcion con retorno de tipo '"
                                << types.types[base]->name
                                << "', recibida funcion con retorno '"
                                << types.types[s->getType()]->name << "'.";
                    logger->error(line, column);
                    
                } else if ( s->params[0] != base ){
                    logger->buf << "Se espera funcion con primer argumento de tipo '"
                                << types.types[base]->name
                                << "', recibido '"
                                << types.types[s->params[0]]->name << "'.";
                    logger->error(line, column);
                    
                }
            } else if ( s->params.size() == 2 ){
                if ( s->getType() != NONE ){
                    logger->buf << "Se espera funcion con retorno vacio.";
                    logger->error(line, column);
                    
                } else if ( s->params[0] != base ){
                    logger->buf << "Se espera funcion con primer argumento de tipo '"
                                << types.types[base]->name
                                << "', recibido '"
                                << types.types[s->params[0]]->name << "'.";
                    logger->error(line, column);
                    
                } else {
                    if ( types.is_base(base) ){
                        if ( typeid(*(types.types[s->params[1]]))
                             == typeid(pointer_descriptor) )
                        {
                            pointer_descriptor* pd =
                               (pointer_descriptor*)(types.types[s->params[1]]);
                            
                            if ( pd->base != base ){
                                logger->buf << "Se espera referencia a '"
                                            << types.types[base]->name
                                            << "', recibida referencia a '"
                                            << types.types[pd->base]->name << "'.";
                                logger->error(line, column);
                            }
                        } else {
                            logger->buf << "Se espera referencia a '"
                                        << types.types[base]->name << "'.";
                            logger->error(line, column);
                        }
                    } else {
                        if ( base != s->params[1] ){
                            if ( typeid(*(types.types[s->params[1]]))
                                 == typeid(pointer_descriptor) )
                            {
                                pointer_descriptor* pd =
                                 (pointer_descriptor*)types.types[s->params[1]];
                                
                                if ( pd->base != base ){
                                    logger->buf << "Se espera referencia a '"
                                                << types.types[base]->name
                                                << "', recibida referencia a '"
                                                << types.types[pd->base]->name << "'.";
                                    logger->error(line, column);
                                }
                            } else {
                                logger->buf << "Se espera referencia a '"
                                            << types.types[base]->name << "'.";
                                logger->error(line, column);
                            }
                        }
                    }
                }
            }
        }
    } else {
        logger->buf << "Identificador '" << function << "' no definido previamente.";
        logger->error(line, column);
    }
}
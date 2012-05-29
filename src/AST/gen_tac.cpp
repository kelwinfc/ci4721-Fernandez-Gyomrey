
void AST_statement::gen_tac(block *b){
    // Unused
    printf("INVALID CALL TO void *AST_statement::gen_tac(block *b)\n");
}

opd *AST_expression::gen_tac(block *b){
    // Unused
    printf("INVALID CALL TO opd AST_expression::gen_tac(block *b)\n");
    return 0;
}

opd *AST_op::gen_tac(block *b){
    switch( oper_type ){
        case AND:
            left->gen_tac(b);
            b->backpatch(left->truelist, b->next_instruction() );
            
            right->gen_tac(b);
            
            falselist.splice(falselist.end(),left->falselist);
            falselist.splice(falselist.end(),right->falselist);
            truelist  = right->truelist;
            break;
            
        case OR:
            left->gen_tac(b);
            b->backpatch(left->falselist, b->next_instruction() );
            
            right->gen_tac(b);
            
            truelist.splice(truelist.end(),left->truelist);
            truelist.splice(truelist.end(),right->truelist);
            falselist  = right->falselist;
            break;
            
        case IMP:
            left->gen_tac(b);
            b->backpatch(left->truelist, b->next_instruction() );
            
            right->gen_tac(b);
            
            truelist.splice(truelist.end(),left->falselist);
            truelist.splice(truelist.end(),right->truelist);
            falselist  = right->falselist;
            break;
            
        case CONSEQ:
            right->gen_tac(b);
            b->backpatch(right->truelist, b->next_instruction() );
            
            left->gen_tac(b);
            
            truelist.splice(truelist.end(),right->falselist);
            truelist.splice(truelist.end(),left->truelist);
            falselist  = left->falselist;
            break;
        
        /* Operadores de comparación de igualdad: ==, != */
        case EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFEQ, l, r, 0, "comparación de igualdad (true)"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "comparación de igualdad (false)"));
            }
            break;
        case UNEQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFNEQ, l, r, 0, "comparación de desigualdad (true)"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "comparación de desigualdad (false)"));
            }
            break;
        
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFL, l, r, 0, "comparación de menor que (true)"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "comparación de menor que (false)"));
            }
            break;
        case LESS_EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFLEQ, l, r, 0, "comparación de menor o igual (true)"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "comparación de menor o igual (false)"));
            }
            break;
        case GREAT:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFG, l, r, 0, "comparación de mayor (true)"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "comparación de mayor (false)"));
            }
            break;
        case GREAT_EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFGEQ, l, r, 0, "comparación de mayor o igual (true)"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "comparación de mayor o igual (false)"));
            }
            break;
        case PLUS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::ADD, d, l, r, "operación binaria de suma"));
                
                return d;
            }
            break;
        case MINUS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::SUB, d, l, r, "operación binaria de resta"));
                
                return d;
            }
            break;
        case PROD:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::MUL, d, l, r, "operación binaria de producto"));
                
                return d;
            }
            break;
        case DIV:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::DIV, d, l, r, "operación binaria de división"));
                
                return d;
            }
            break;
        case MOD:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::MOD, d, l, r, "operación binaria de módulo"));
                
                return d;
            }
            break;
        default:
            break;
    }
    return 0;
}

opd *AST_un_op::gen_tac(block *b){
    opd* e = expr->gen_tac(b);
    
    switch( oper_type ){
        case NOT:
            falselist = expr->truelist;
            truelist  = expr->falselist;
            break;
        default:
            {
                opd* d = new opd();
                b->append_inst(new quad(quad::UMINUS, d, e, 0, "operación binaria de inverso"));
                
                return d;
            }
            break;
    }
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
    opd *t = new opd();
    b->append_inst(new quad(quad::CP, t, new opd(offset), 0, string("dirección relativa del string en la tabla \"") + strings.to_string(value) + "\""));
    return t;
}

opd *AST_enum_constant::gen_tac(block *b){
    return new opd(enum_index);
}

opd *AST_boolean::gen_tac(block *b){
    if ( value ){
        truelist.clear();
        truelist.push_back(b->next_instruction());
    } else {
        falselist.clear();
        falselist.push_back(b->next_instruction());
    }
    b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de booleano"));
    return 0;
}

opd *AST_lval::gen_tac(block *b){
    int sta_base = 0;
    opd *din_base = gen_tac_lval(b, &sta_base);
    return gen_tac_lval_disp(b, din_base, &sta_base);
}

opd *AST_lval::gen_tac_lval(block *b, int *sta_base){
    // Unused
    printf("INVALID CALL TO opd AST_lval::gen_tac_lval(block *b, int *sta_base)\n");
    return 0;
}

opd *AST_lval::gen_tac_lval_disp(block *b, opd *din_base, int *sta_base){
    opd *t;
    if (0 != *sta_base && O_TEMP != din_base->type) {
        t = new opd();
    } else {
        t = din_base;// nuevo resultado de las operaciones
    }
    if (0 != *sta_base) {
        b->append_inst(new quad(quad::ADD, t, din_base, new opd(*sta_base), "sumar base estática a base dinámica (no se pudo retrasar más la suma)"));
    }
    return t;
}

opd *AST_lval::gen_tac_arr(block* b, int *sta_base, opd **ind_addr, int *arr_base){
    // el lval en el que estamos parados es un arreglo. la base estática comienza por el lower index
    *arr_base = ((array_descriptor*)types.types[ type ])->lower_index;
    return gen_tac_lval(b, sta_base);
}

opd *AST_ident::gen_tac_lval(block* b, int *sta_base){
    *sta_base = 0;
    return new opd(sym);
}

opd *AST_dereference::gen_tac_lval(block *b, int *sta_base){
    opd *t, *din_base = value->gen_tac_lval(b, sta_base);
    din_base = gen_tac_lval_disp(b, din_base, sta_base);
    if (O_TEMP != din_base->type) {
        t = new opd();
    } else {
        t = din_base;
    }
    b->append_inst(new quad(quad::LD, t, din_base, 0, "dereferenciar un lvalue"));
    *sta_base = 0;
    return t;
}

/**
 * AST_address no es más que un contenedor para especificar que no se debe
 * dereferenciar el apuntador devuelto por el AST_lval
 */
opd *AST_address::gen_tac_lval(block *b, int *sta_base){
    return value->gen_tac_lval(b, sta_base);
}

/**
 * índice más derecho de un acceso de arreglo
 */
opd *AST_array_access::gen_tac_lval(block *b, int *sta_base){
    int arr_base = 0;
    opd **ind_addr = new opd*();
    *ind_addr = 0;// indicar que alguien va a tener que colocar aquí la dirección apropiada
    opd *din_base = gen_tac_arr(b, sta_base, ind_addr, &arr_base);

    type_descriptor* ad = types.types[ type ];
    *sta_base -= arr_base * ad->width;

    // multiplicar la suma de índices por el ancho de lo que alberga el arreglo
    b->append_inst(new quad(quad::MUL, *ind_addr, *ind_addr, new opd(ad->width), "multiplicar el aplanamiento de índices por el ancho del tipo"));

    // sumar la posición del arreglo (din_base) con el índice dinámico (ind_base)
    b->append_inst(new quad(quad::ADD, *ind_addr, din_base, *ind_addr, "sumar la base del arreglo con el desplazamiento hasta la posición solicitada"));

    din_base = *ind_addr;
    delete ind_addr;

    return din_base;
}

opd *AST_array_access::gen_tac_arr(block *b, int *sta_base, opd **ind_addr, int *arr_base){
    opd *din_base = value->gen_tac_arr(b, sta_base, ind_addr, arr_base);

    if (0 == *ind_addr) {
        // no se hace backpatck porque sabemos que es un entero
        *ind_addr = index->gen_tac(b);
        if (O_TEMP != (*ind_addr)->type) {
            opd *t = new opd();
            b->append_inst(new quad(quad::CP, t, *ind_addr, 0, "el índice más izquierdo de un arreglo es entero, se copia a temporal para poder operar sobre él"));
            *ind_addr = t;
        }
    } else {
        // (i1 x n2) + i2 >>= i1 (parte de la suma)
        b->append_inst(new quad(quad::ADD, *ind_addr, *ind_addr, index->gen_tac(b), "parte dinámica del acceso a arreglo. sumar al acumulador el índice dinámico"));
    }

    if (typeid(*types.types[ type ]) == typeid(array_descriptor)){
        array_descriptor* ad = (array_descriptor*)types.types[ type ];

        // (i1 x n2) + i2 (parte del producto)
        b->append_inst(new quad(quad::MUL, *ind_addr, *ind_addr, new opd(ad->num_elements), "como aún nos encontramos en un arreglo, multiplicar el índice dinámico por el tamaño del siguiente índice a la derecha"));

        // (l1 x n2) + l2
        *arr_base = *arr_base * ad->num_elements + ad->lower_index;
    }

    return din_base;
}

opd *AST_struct_access::gen_tac_lval(block *b, int *sta_base){
    opd *din_base = value->gen_tac_lval(b, sta_base);
    // TODO al asignar el espacio de memoria al símbolo, se debe colocar un cero en el entero de chequeo
    if (union_access) {
        union_type *ud = (union_type*)types.types[ sym->getType() ];
        din_base = gen_tac_lval_disp(b, din_base, sta_base);
        opd *t1 = new opd(), *t2 = new opd((int)sym->index);
        b->append_inst(new quad(quad::LD, t1, din_base, new opd(ud->width - 4), "cargando identificador de tipo usado en el union"));
        b->append_inst(new quad(quad::IFEQ, t1, t2, new opd(b->next_instruction() + 1, 1), "en caso estar actualizado el tipo usado, se mantiene"));
        // TODO write "Se trató de utilizar el tipo X pero el activo era el Y en (L, C)" ERR y modificar el next_instruction
        b->append_inst(new quad(quad::ST, t2, din_base, new opd(ud->width - 4),
            "guardando nuevo identificador de tipo usado en el union"));
    }
    *sta_base += sym->offset;
    return din_base;
}

opd *AST_rlval::gen_tac(block *b) {
    opd *l = value->gen_tac(b);
    // Al copiar apuntadores, sólo el caso del @ no se sabe cuál es el apuntador al apuntador (ej. variable global)
    if (typeid(*value) != typeid(AST_address)) {
        opd *t;
        if (O_TEMP != l->type) {
            t = new opd();
        } else {
            t = l;
        }
        b->append_inst(new quad(quad::LD, t, l, 0, "cargar el valor de un lvalue usado del lado derecho"));
        l = t;
    }
    return l;
}

opd *AST_conversion::gen_tac(block *b){
    opd *v = expr->gen_tac(b);
    if (0 == v) {
        v = new opd();
    } else if (O_TEMP != v->type) {
        opd *t = new opd();
        b->append_inst(new quad(quad::CP, t, v, 0, "la conversión de tipos requiere a juro un temporal"));
        v = t;
    }
    if (BOOLEAN == expr->type) {
        b->backpatch(expr->truelist, b->next_instruction() );
        b->append_inst(new quad(quad::CP, v, new opd(true), 0, "asignación de valor booleano (true) obtenido al lvalue"));
        b->append_inst(new quad(quad::GOTO, 0, 0, new opd(b->next_instruction() + 2, 1), "salto después de asignar el valor booleano"));

        b->backpatch( expr->falselist, b->next_instruction() );
        b->append_inst(new quad(quad::CP, v, new opd(false), 0, "asignación de valor booleano (false) obtenido al lvalue"));
    }
    b->append_inst(new quad(quad::CONV, v, v, 0, string("conversión de tipo ") + PRINT_TYPE(original_type) + " a " + PRINT_TYPE(type)));
    return v;
}

opd *AST_function_call::gen_tac(block *b){
    params->gen_tac(b);
    opd *t = new opd();
    b->append_inst(new quad(quad::CALL, t, new opd(sym), 0, "llamada de función guardada en el primer argumento"));
    return t;
}

void AST_block::gen_tac(block *b){
    uint nsize = statements.size();
    
    for ( uint i = 0; i != nsize; i++){
        if ( i > 0 ){
            b->backpatch(statements[i-1]->next_list, b->next_instruction());
        }
        statements[i]->gen_tac(b);
        
        continue_list.splice(continue_list.end(), statements[i]->continue_list);
        return_list.splice(return_list.end(), statements[i]->return_list);
        break_list.splice(break_list.end(), statements[i]->break_list);
    }
    if ( nsize > 0 ){
        next_list = statements.back()->next_list;
    }
}

void AST_parameters_list::gen_tac(block* b){
    for (vector< AST_expression* >::iterator it = elem.begin(); it != elem.end(); ++ it) {
        opd *v = (*it)->gen_tac(b);
        if ((*it)->type == BOOLEAN) {
            b->backpatch((*it)->truelist, b->next_instruction() );
            b->append_inst(new quad(quad::PARAM, new opd(true), 0, 0, "argumento de función booleano (true)"));
            b->append_inst(new quad(quad::GOTO, 0, 0, new opd(b->next_instruction() + 2, 1), "salto después de asignar el valor booleano"));

            b->backpatch( (*it)->falselist, b->next_instruction() );
            b->append_inst(new quad(quad::PARAM, new opd(false), 0, 0, "argumento de función booleano (false)"));
        } else {
            b->append_inst(new quad(quad::PARAM, v, 0, 0, "argumento de función no booleano"));
        }
    }
}

void AST_declaration::gen_tac(block *b){
    // Unused
    printf("INVALID CALL TO void AST_declaration::gen_tac(block *b)\n");
}

void AST_variable_declaration::gen_tac(block *b){
    next_list.clear();
    // TODO si aquí es donde se asigna el espacio, aquí debería agregarse el indicador de tipo de union a cero
    if (0 == value) {
        return;
    }
    opd* l = new opd(sym);
    opd* r = value->gen_tac(b);
    
    if ( value->type == BOOLEAN){

        b->backpatch( value->truelist, b->next_instruction() );
        b->append_inst(new quad(quad::ST, l, new opd(true), 0, "declaración con asignación del valor booleano obtenido (true) al lvalue"));

        next_list.push_back( b->next_instruction() );
        b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto después de declarar y asignar el valor booleano"));

        b->backpatch( value->falselist, b->next_instruction() );
        b->append_inst(new quad(quad::ST, l, new opd(false), 0, "declaración con asignación de valor booleano obtenido al lvalue"));
    } else {
        b->append_inst(new quad(quad::ST, l, r, 0, "declaración con asignación de un valor de cualquier tipo obtenido al lvalue"));
    }
}

void AST_arg_list::gen_tac(block *b){
    // TAC no asigna los valores recibidos por PARAM. se asume que ya están en los respectivos símbolos
}

void AST_discrete_arg_list::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_discrete_arg_list::gen_tac(block *b)\n");
}

void AST_function::gen_tac(block *b){
    b->append_inst(new quad(quad::PROLOGUE,  0, 0, new opd(func),
                            "prologo de funcion" + func->getName() )
                  );
    
    instructions->gen_tac(b);
    
    b->backpatch( instructions->next_list, b->next_instruction() );
    b->backpatch( instructions->return_list, b->next_instruction() );
    
    b->append_inst(new quad(quad::EPILOGUE,  0, 0, new opd(func),
                            "epilogo de funcion" + func->getName() )
                  );
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
    next_list.clear();

    opd* l = lvalue->gen_tac(b);
    opd* r = expr->gen_tac(b);
    
    if ( expr->type == BOOLEAN){

        b->backpatch( expr->truelist, b->next_instruction() );
        b->append_inst(new quad(quad::ST, l, new opd(true), 0, "asignación del valor booleano obtenido (true) al lvalue"));

        next_list.push_back( b->next_instruction() );
        b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto después de asignar el valor booleano"));

        b->backpatch( expr->falselist, b->next_instruction() );
        b->append_inst(new quad(quad::ST, l, new opd(false), 0, "asignación de valor booleano obtenido al lvalue"));
    } else {
        b->append_inst(new quad(quad::ST, l, r, 0, "asignación de un valor de cualquier tipo obtenido al lvalue"));
    }
}

void AST_return::gen_tac(block *b){
    //TODO verificar si es booleano
    opd* r = 0;
    if ( expr ){
        r = expr->gen_tac(b);
    }
    
    return_list.push_back(b->next_instruction());
    b->append_inst(new quad(quad::RETURN, r, 0, 0, "retorno de funcion"));
}

void AST_conditional::gen_tac(block *b){
    
    // Si la rama actual tiene condicion
    if ( expr ){
        // Codigo que verifica la condicion del if
        expr->gen_tac(b);
        
        // Se marcan los saltos positivos a la siguiente instruccion, donde
        // empezara el codigo de la primera rama
        b->backpatch(expr->truelist, b->next_instruction());
        
        // Codigo de la primera rama
        blck->gen_tac(b);
        
        // Si el if tiene otra rama
        if ( else_if ){
            // generar un goto proveniente de la ejecucion de la rama anterior
            // agregar el goto a la nextlist de este condicional
            next_list.push_back( b->next_instruction() );
            b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de else_if"));
            
            // marcar los saltos de la condicion negativa del if a la siguiente
            // instruccion donde se alojara la verificacion del codigo de esta
            b->backpatch(expr->falselist, b->next_instruction());
            else_if->gen_tac(b);
            
            next_list.splice(next_list.end(), else_if->next_list);
            continue_list.splice(continue_list.end(), else_if->continue_list);
            break_list.splice(break_list.end(), else_if->break_list);
            return_list.splice(return_list.end(), else_if->return_list);
            
        } else {
            next_list.splice(next_list.end(), expr->falselist);
        }
    } else {
        // Rama else
        blck->gen_tac(b);
    }
    
    next_list.splice(next_list.end(), blck->next_list);
    continue_list.splice(continue_list.end(), blck->continue_list);
    break_list.splice(break_list.end(), blck->break_list);
    return_list.splice(return_list.end(), blck->return_list);
}

void AST_loop::gen_tac(block *b){
    next_list.clear();
    int start_instr = b->next_instruction(); // etiqueta para el salto de la
                                            // ultima instruccion del loop
    
    expr->gen_tac(b);
    b->backpatch(expr->truelist, b->next_instruction());
    blck->gen_tac(b);
    
    // enviar los saltos de los continue hasta el principio del codigo de
    // evaluacion de la condicion
    b->backpatch(blck->continue_list, start_instr);
    
    b->append_inst(new quad(quad::GOTO, 0, 0, new opd(start_instr, true), "salto condicional de un loop"));
    
    next_list = expr->falselist;
    next_list.splice(next_list.end(), blck->break_list);
}

void AST_bounded_loop::gen_tac(block *b){
    next_list.clear();

    opd* l = left_bound->gen_tac(b);
    opd* r = right_bound->gen_tac(b);
    opd* i = new opd(), *os = new opd(sym);
    // prólogo
    b->append_inst(new quad(quad::CP, i, new opd(-1), 0, "se asume que se va a restar en cada ciclo"));
    b->append_inst(new quad(quad::IFGEQ, l, r, new opd(b->next_instruction() + 2,true), "en este caso, efectivamente, se resta en cada ciclo"));
    b->append_inst(new quad(quad::CP, i, new opd(1), 0, "si se alcanzó esta instrucción, se va a sumar en cada ciclo"));
    int start_instr = b->next_instruction();
    // comienzo del ciclo
    b->append_inst(new quad(quad::IFEQ, i, new opd(1), new opd(b->next_instruction() + 3,true), "verificar que se suma por ciclo"));
    b->append_inst(new quad(quad::IFGEQ, l, r, new opd(b->next_instruction() + 4,true),
        "dado que se resta por ciclo, verificar que no se alcanzó el mínimo derecho"));
    next_list.push_back( b->next_instruction() );
    b->append_inst(new quad(quad::GOTO, 0, 0, 0, "si se llegó hasta aquí, es porque el ciclo finalizó naturalmente"));
    b->append_inst(new quad(quad::IFLEQ, l, r, new opd(b->next_instruction() + 2,true),
        "dado que se suma por ciclo, verificar que no se alcanzó el máximo derecho"));
    next_list.push_back( b->next_instruction() );
    b->append_inst(new quad(quad::GOTO, 0, 0, 0, "si se llegó hasta aquí, es porque el ciclo finalizó naturalmente"));
    // ciclo
    blck->gen_tac(b);
    // si ocurre un continue en el ciclo
    b->backpatch(blck->continue_list, start_instr);
    // si ocurre un break en el ciclo
    next_list.splice(next_list.end(), blck->break_list);

    b->append_inst(new quad(quad::ADD, os, new opd(1), 0, "tanto para enteros como para caracteres sumar uno hasta llegars"));
    // comenzar de nuevo
    b->append_inst(new quad(quad::GOTO, 0, 0, new opd(start_instr, true), "volver a comenzar el for acotado"));
}

void AST_break::gen_tac(block *b){
    break_list.push_back(b->next_instruction());
    b->append_inst(new quad(quad::GOTO, 0, 0, 0, "break"));
}

void AST_continue::gen_tac(block *b){
    continue_list.push_back(b->next_instruction());
    b->append_inst(new quad(quad::GOTO, 0, 0, 0, "continue"));
}

void AST_read::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_read::gen_tac(block *b)\n");
}

void AST_print::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_print::gen_tac(block *b)\n");
}

void AST_fill::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_fill::gen_tac(block *b)\n");
}

void AST_map::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_map::gen_tac(block *b)\n");
}

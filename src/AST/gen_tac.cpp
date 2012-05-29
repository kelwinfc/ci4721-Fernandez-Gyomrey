
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
                
                b->append_inst(new quad(quad::IFEQ, l, r, 0, "comparación de igualdad"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional comparación de igualdad"));
            }
            break;
        case UNEQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFNEQ, l, r, 0, "comparación de desigualdad"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de comparación de desigualdad"));
            }
            break;
        
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFL, l, r, 0, "comparación de menor que"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de comparación de menor qué"));
            }
            break;
        case LESS_EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFLEQ, l, r, 0, "comparación de menor o igual"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de comparación de menor o igual"));
            }
            break;
        case GREAT:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFG, l, r, 0, "comparación de mayor"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de comparación de mayor"));
            }
            break;
        case GREAT_EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFGEQ, l, r, 0, "comparación de mayor o igual"));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional de comparación de mayor o igual"));
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
    expr->gen_tac(b);
    
    switch( oper_type ){
        case NOT:
            falselist = expr->truelist;
            truelist  = expr->falselist;
            break;
        default:
            {
                opd* e = expr->gen_tac(b);
                
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
    return gen_tac_lval_disp(b, din_base, sta_base);
}

opd *AST_lval::gen_tac_lval(block *b, int *sta_base){
    // Unused
    printf("INVALID CALL TO opd AST_lval::gen_tac_lval(block *b, int *sta_base)\n");
    return 0;
}

opd *AST_lval::gen_tac_lval_disp(block *b, opd *din_base, int sta_base){
    opd *t;
    if (0 != sta_base && O_TEMP != din_base->type) {
        t = new opd();
    } else {
        t = din_base;// nuevo resultado de las operaciones
    }
    if (0 != sta_base) {
        b->append_inst(new quad(quad::ADD, t, din_base, new opd(sta_base), "sumar base estática a base dinámica (no se pudo retrasar más la suma)"));
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
    if ( sym->getType() == BOOLEAN ){
        truelist.push_back( b->next_instruction() );
        falselist.push_back( b->next_instruction() + 1);
        
        b->append_inst(new quad(quad::IF, new opd(sym), 0, 0, "identificador de tipo booleano"));
        b->append_inst(new quad(quad::GOTO, 0, 0, 0, "identificador de tipo booleano"));
    }
    return new opd(sym);
}

opd *AST_dereference::gen_tac_lval(block *b, int *sta_base){
    opd *t, *din_base = value->gen_tac_lval(b, sta_base);
    din_base = gen_tac_lval_disp(b, din_base, *sta_base);
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
    *sta_base += sym->offset;
    return din_base;
}

opd *AST_conversion::gen_tac(block *b){
    opd *v = expr->gen_tac(b);
    if (O_TEMP != v->type) {
        opd *t = new opd();
        b->append_inst(new quad(quad::CP, t, v, 0, "la conversión de tipos requiere a juro un temporal"));
        v = t;
    }
    b->append_inst(new quad(quad::CP, v, v, 0, string("conversión de tipo ") + PRINT_TYPE(original_type) + " a " + PRINT_TYPE(type)));
    return v;
}

void AST_block::gen_tac(block *b){
    uint nsize = statements.size();
    
    for ( uint i = 0; i != nsize; i++){
        if ( i > 0 ){
            b->backpatch(statements[i-1]->next_list, b->next_instruction());
        }
        statements[i]->gen_tac(b);
        continue_list.splice(continue_list.end(), statements[i]->continue_list);
        break_list.splice(break_list.end(), statements[i]->break_list);
    }
    if ( nsize > 0 ){
        next_list = statements.back()->next_list;
    }
}

void AST_parameters_list::gen_tac(block* b){
    for (vector< AST_expression* >::iterator it = elem.begin(); it != elem.end(); ++ it) {
        b->append_inst(new quad(quad::PARAM, (*it)->gen_tac(b), 0, 0, "argumento de función"));
    }
}

opd *AST_function_call::gen_tac(block *b){
    params->gen_tac(b);
    opd *t = new opd();
    b->append_inst(new quad(quad::CALL, t, new opd(sym), 0, "llamada de función guardada en el primer argumento"));
    return t;
}

void AST_declaration::gen_tac(block *b){
    // Unused
    printf("INVALID CALL TO void AST_declaration::gen_tac(block *b)\n");
}

void AST_variable_declaration::gen_tac(block *b){
    if (0 != value) {
        b->append_inst(new quad(quad::CP, new opd(sym), value->gen_tac(b), 0, "declaración de variable"));
        next_list.clear();
    }
}

void AST_arg_list::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_arg_list::gen_tac(block *b)\n");
}

void AST_discrete_arg_list::gen_tac(block *b){
    printf("UNIMPLEMENTED void AST_discrete_arg_list::gen_tac(block *b)\n");
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
    next_list.clear();
    
    opd* l = lvalue->gen_tac(b);
    b->backpatch(lvalue->truelist, b->next_instruction());
    b->backpatch(lvalue->falselist, b->next_instruction());
    
    opd* r = expr->gen_tac(b);
    
    if ( expr->type == BOOLEAN){
        
        b->backpatch( expr->truelist, b->next_instruction() );
        b->append_inst(new quad(quad::CP, l, new opd(true), 0, "asignación del valor booleano obtenido al lvalue"));
        
        next_list.push_back( b->next_instruction() );
        b->append_inst(new quad(quad::GOTO, 0, 0, 0, "salto condicional después de asignar el valor booleano"));
        
        b->backpatch( expr->falselist, b->next_instruction() );
        b->append_inst(new quad(quad::CP, l, new opd(false), 0, "asignación de valor booleano obtenido al lvalue"));
    } else {
        // Al copiar apuntadores, sólo el caso del @ no se sabe cuál es el apuntador al apuntador (ej. variable global)
        quad::OP op = typeid(*r) == typeid(AST_address) ? quad::CP : quad::SW;
        b->append_inst(new quad(op, l, r, 0, "asignación de un valor escalar obtenido al lvalue"));
    }
}

void AST_return::gen_tac(block *b){
    
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
        } else {
            next_list.splice(next_list.end(), expr->falselist);
        }
    } else {
        // Rama else
        blck->gen_tac(b);
    }
    next_list.splice(next_list.end(), blck->next_list);
}

void AST_loop::gen_tac(block *b){
    
    int next_instr = b->next_instruction(); // etiqueta para el salto de la
                                            // ultima instruccion del loop
    
    expr->gen_tac(b);
    b->backpatch(expr->truelist, b->next_instruction());
    blck->gen_tac(b);
    
    // enviar los saltos de los continue hasta el principio del codigo de
    // evaluacion de la condicion
    b->backpatch(blck->continue_list, next_instr);
    
    b->append_inst(new quad(quad::GOTO, 0, 0, new opd(next_instr, true), "salto condicional de un loop"));
    
    next_list = expr->falselist;
    next_list.splice(next_list.end(), blck->break_list);
}

void AST_bounded_loop::gen_tac(block *b){
    /*
    opd* l = left_bound->gen_tac(b);
    b->append_inst(new quad(quad::CP, new opd(sym), l));
    
    int next_instr = b->next_instruction();
    opd* r = right_bound->gen_tac(b);
    //TODO completar marcar salto negativo a final de ciclo
    b->append_inst(new quad(quad::IFLEQ, l, r, new opd(b->next_instruction()+2,true)));
    b->append_inst(new quad(quad::GOTO, 0, 0, 0));
    
    blck->gen_tac(b);
    b->append_inst(new quad(quad::GOTO, 0, 0, new opd(next_instr, true)));
    */
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

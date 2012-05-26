
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
                
                b->append_inst(new quad(quad::IFEQ, l, r, 0));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0));
            }
            break;
        case UNEQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFNEQ, l, r, 0));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0));
            }
            break;
        
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFL, l, r, 0));
                b->append_inst(new quad(quad::GOTO, 0,0, 0));
            }
            break;
        case LESS_EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFLEQ, l, r, 0));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0));
            }
            break;
        case GREAT:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFG, l, r, 0));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0));
            }
            break;
        case GREAT_EQ:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                truelist.push_back( b->next_instruction() );
                falselist.push_back( b->next_instruction() + 1);
                
                b->append_inst(new quad(quad::IFGEQ, l, r, 0));
                b->append_inst(new quad(quad::GOTO, 0, 0, 0));
            }
            break;
        case PLUS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::ADD, d, l, r));
                
                return d;
            }
            break;
        case MINUS:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::SUB, d, l, r));
                
                return d;
            }
            break;
        case PROD:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::MUL, d, l, r));
                
                return d;
            }
            break;
        case DIV:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::DIV, d, l, r));
                
                return d;
            }
            break;
        case MOD:
            {
                opd* l = left->gen_tac(b);
                opd* r = right->gen_tac(b);
                
                opd* d = new opd();
                b->append_inst(new quad(quad::MOD, d, l, r));
                
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
                b->append_inst(new quad(quad::UMINUS, d, e));
                
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
    // TODO en función de una tabla realizar la asignación del apuntador
    // al string. sólo se requiere el valor completo del string al imprimir
    printf("UNIMPLEMENTED opd *AST_string::gen_tac(block *b)\n");
    return 0;
}

opd *AST_enum_constant::gen_tac(block *b){
    // TODO las constantes deben tener un índice. esto pasa a ser un número
    // a bajo nivel.
    printf("UNIMPLEMENTED opd *AST_enum_constant::gen_tac(block *b)\n");
    return 0;
}

opd *AST_boolean::gen_tac(block *b){
    if ( value ){
        truelist.clear();
        truelist.push_back(b->next_instruction());
    } else {
        falselist.clear();
        falselist.push_back(b->next_instruction());
    }
    b->append_inst(new quad(quad::GOTO, 0, 0, 0));
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

opd *AST_lval::gen_tac_lval_disp(block *b, opd *din_base, int abs_base){
    opd *t;
    if (0 != abs_base && O_TEMP != din_base->type) {
        t = new opd();
    } else {
        t = din_base;// nuevo resultado de las operaciones
    }
    if (0 != abs_base) {
        b->append_inst(new quad(quad::ADD, t, din_base, new opd(abs_base)));// sumar primero si hace falta
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
        
        b->append_inst(new quad(quad::IF, new opd(sym)));
        b->append_inst(new quad(quad::GOTO, 0, 0, 0));
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
    b->append_inst(new quad(quad::LD, t, din_base));
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
    b->append_inst(new quad(quad::MUL, *ind_addr, *ind_addr, new opd(ad->width)));

    // sumar la posición del arreglo (din_base) con el índice dinámico (ind_base)
    b->append_inst(new quad(quad::MUL, *ind_addr, din_base, *ind_addr));

    din_base = *ind_addr;
    delete ind_addr;

    return din_base;
}

opd *AST_array_access::gen_tac_arr(block *b, int *sta_base, opd **ind_addr, int *arr_base){
    opd *din_base = value->gen_tac_arr(b, sta_base, ind_addr, arr_base);

    if (0 == *ind_addr) {
        *ind_addr = index->gen_tac(b);
    } else {
        // (i1 x n2) + i2 >>= i1 (parte de la suma)
        b->append_inst(new quad(quad::ADD, *ind_addr, *ind_addr, index->gen_tac(b)));
    }

    if (typeid(*types.types[ type ]) == typeid(array_descriptor)){
        array_descriptor* ad = (array_descriptor*)types.types[ type ];

        // (i1 x n2) + i2 (parte del producto)
        b->append_inst(new quad(quad::MUL, *ind_addr, *ind_addr, new opd(ad->num_elements)));

        // (l1 x n2) + l2
        *arr_base = *arr_base * ad->num_elements + ad->lower_index;
    }

    return din_base;
}

opd *AST_struct_access::gen_tac_lval(block *b, int *sta_base){
    opd *din_base = value->gen_tac_lval(b, sta_base);
    sta_base += sym->offset;
    return din_base;
}

opd *AST_conversion::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_conversion::gen_tac(block *b)\n");
    return 0;
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
    printf("UNIMPLEMENTED opd *AST_parameters_list::gen_tac(block *b)\n");
}

opd *AST_function_call::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_function_call::gen_tac(block *b)\n");
    return 0;
}

void AST_declaration::gen_tac(block *b){
    printf("INVALID CALL TO opd *AST_declaration::gen_tac(block *b)\n");
}

void AST_variable_declaration::gen_tac(block *b){
    if (0 != value) {
        b->append_inst(new quad(quad::CP, new opd(sym), value->gen_tac(b)));
        next_list.clear();
    }
}

void AST_arg_list::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_arg_list::gen_tac(block *b)\n");
}

void AST_discrete_arg_list::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_discrete_arg_list::gen_tac(block *b)\n");
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
        b->append_inst(new quad(quad::CP, l, new opd(true)));
        
        next_list.push_back( b->next_instruction() );
        b->append_inst(new quad(quad::GOTO, 0, 0, 0));
        
        b->backpatch( expr->falselist, b->next_instruction() );
        b->append_inst(new quad(quad::CP, l, new opd(false)));
    } else {
        b->append_inst(new quad(quad::CP, l, r));
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
            b->append_inst(new quad(quad::GOTO, 0, 0, 0));
            
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
    
    b->append_inst(new quad(quad::GOTO, 0, 0, new opd(next_instr, true)));
    
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
    b->append_inst(new quad(quad::GOTO, 0, 0, 0));
}

void AST_continue::gen_tac(block *b){
    continue_list.push_back(b->next_instruction());
    b->append_inst(new quad(quad::GOTO, 0, 0, 0));
}

void AST_read::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_assignment::gen_tac(block *b)\n");
}

void AST_print::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_print::gen_tac(block *b)\n");
}

void AST_fill::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_fill::gen_tac(block *b)\n");
}

void AST_map::gen_tac(block *b){
    printf("UNIMPLEMENTED opd *AST_map::gen_tac(block *b)\n");
}
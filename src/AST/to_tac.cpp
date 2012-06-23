#include <queue>

map<string,int> func_to_prologue;
map<string,int> func_to_epilogue;
extern unsigned int first_instruction;

void AST_statement::to_tac(Tac* t){
    // Unused
    throw "INVALID CALL TO void *AST_statement::to_tac(Tac* t)";
}

opd *AST_expression::to_tac(Tac* t){
    // Unused
    throw "INVALID CALL TO opd *AST_expression::to_tac(Tac* t)";
}

opd *AST_op::to_tac(Tac* t){
    switch( oper_type ){
        case AND:
            left->to_tac(t);
            t->backpatch(left->truelist, t->next_instruction() );
            
            right->to_tac(t);
            
            falselist.splice(falselist.end(),left->falselist);
            falselist.splice(falselist.end(),right->falselist);
            truelist  = right->truelist;
            break;
            
        case OR:
            left->to_tac(t);
            t->backpatch(left->falselist, t->next_instruction() );
            
            right->to_tac(t);
            
            truelist.splice(truelist.end(),left->truelist);
            truelist.splice(truelist.end(),right->truelist);
            falselist  = right->falselist;
            break;
            
        case IMP:
            left->to_tac(t);
            t->backpatch(left->truelist, t->next_instruction() );

            right->to_tac(t);

            truelist.splice(truelist.end(),left->falselist);
            truelist.splice(truelist.end(),right->truelist);
            falselist  = right->falselist;
            break;

        case CONSEQ:
            right->to_tac(t);
            t->backpatch(right->truelist, t->next_instruction() );

            left->to_tac(t);

            truelist.splice(truelist.end(),right->falselist);
            truelist.splice(truelist.end(),left->truelist);
            falselist  = left->falselist;
            break;
        
        /* Operadores de comparación de igualdad: ==, != */
        case EQ:
            {
                opd* l = left->to_tac(t);

                if (BOOLEAN == left->type) {
                    l = new opd();
                    // almacenar en un temporal el valor booleano de la rama izquierda
                    t->backpatch( left->truelist, t->next_instruction() );
                    t->append_quad(quad::CP, l, new opd(true), 0, "asignación del valor booleano obtenido (true) a rama izquierda de igualdad");
                    t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "salto después de asignar el valor booleano");

                    t->backpatch( left->falselist, t->next_instruction() );
                    t->append_quad(quad::CP, l, new opd(false), 0, "asignación del valor booleano obtenido (false) a rama izquierda de igualdad");
                }
                
                opd* r = right->to_tac(t);

                if (BOOLEAN == right->type) {
                    r = new opd();
                    // almacenar en un temporal el valor booleano de la rama derecha
                    t->backpatch( right->truelist, t->next_instruction() );
                    t->append_quad(quad::CP, r, new opd(true), 0, "asignación del valor booleano obtenido (true) a rama derecha de igualdad");
                    t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "salto después de asignar el valor booleano");

                    t->backpatch( right->falselist, t->next_instruction() );
                    t->append_quad(quad::CP, r, new opd(false), 0, "asignación del valor booleano obtenido (false) a rama derecha de igualdad");
                }

                truelist.push_back( t->next_instruction() );
                t->append_quad(quad::IFEQ, l, r, 0, "comparación de igualdad (true)");
                falselist.push_back( t->next_instruction() );
                t->append_quad(quad::GOTO, 0, 0, 0, "comparación de igualdad (false)");
            }
            break;
        case UNEQ:
            {
                opd* l = left->to_tac(t);

                if (BOOLEAN == left->type) {
                    l = new opd();
                    // almacenar en un temporal el valor booleano de la rama izquierda
                    t->backpatch( left->truelist, t->next_instruction() );
                    t->append_quad(quad::CP, l, new opd(true), 0, "asignación del valor booleano obtenido (true) a rama izquierda de desigualdad");
                    t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "salto después de asignar el valor booleano");

                    t->backpatch( left->falselist, t->next_instruction() );
                    t->append_quad(quad::CP, l, new opd(false), 0, "asignación del valor booleano obtenido (false) a rama izquierda de desigualdad");
                }
                
                opd* r = right->to_tac(t);

                if (BOOLEAN == right->type) {
                    r = new opd();
                    // almacenar en un temporal el valor booleano de la rama derecha
                    t->backpatch( right->truelist, t->next_instruction() );
                    t->append_quad(quad::CP, r, new opd(true), 0, "asignación del valor booleano obtenido (true) a rama derecha de desigualdad");
                    t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "salto después de asignar el valor booleano");

                    t->backpatch( right->falselist, t->next_instruction() );
                    t->append_quad(quad::CP, r, new opd(false), 0, "asignación del valor booleano obtenido (false) a rama derecha de desigualdad");
                }
                
                truelist.push_back( t->next_instruction() );
                t->append_quad(quad::IFNEQ, l, r, 0, "comparación de desigualdad (true)");
                falselist.push_back( t->next_instruction() );
                t->append_quad(quad::GOTO, 0, 0, 0, "comparación de desigualdad (false)");
            }
            break;
        
        /* Operadores de relación de orden: <, <=, >, >= */
        case LESS:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                truelist.push_back( t->next_instruction() );
                falselist.push_back( t->next_instruction() + 1);
                
                t->append_quad(quad::IFL, l, r, 0, "comparación de menor que (true)");
                t->append_quad(quad::GOTO, 0, 0, 0, "comparación de menor que (false)");
            }
            break;
        case LESS_EQ:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                truelist.push_back( t->next_instruction() );
                falselist.push_back( t->next_instruction() + 1);
                
                t->append_quad(quad::IFLEQ, l, r, 0, "comparación de menor o igual (true)");
                t->append_quad(quad::GOTO, 0, 0, 0, "comparación de menor o igual (false)");
            }
            break;
        case GREAT:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                truelist.push_back( t->next_instruction() );
                falselist.push_back( t->next_instruction() + 1);
                
                t->append_quad(quad::IFG, l, r, 0, "comparación de mayor (true)");
                t->append_quad(quad::GOTO, 0, 0, 0, "comparación de mayor (false)");
            }
            break;
        case GREAT_EQ:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                truelist.push_back( t->next_instruction() );
                falselist.push_back( t->next_instruction() + 1);
                
                t->append_quad(quad::IFGEQ, l, r, 0, "comparación de mayor o igual (true)");
                t->append_quad(quad::GOTO, 0, 0, 0, "comparación de mayor o igual (false)");
            }
            break;
        case PLUS:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                opd* d = new opd();
                t->append_quad(quad::ADD, d, l, r, "operación binaria de suma");
                
                return d;
            }
            break;
        case MINUS:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                opd* d = new opd();
                t->append_quad(quad::SUB, d, l, r, "operación binaria de resta");
                
                return d;
            }
            break;
        case PROD:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                opd* d = new opd();
                t->append_quad(quad::MUL, d, l, r, "operación binaria de producto");
                
                return d;
            }
            break;
        case DIV:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                opd* d = new opd();
                t->append_quad(quad::DIV, d, l, r, "operación binaria de división");
                
                return d;
            }
            break;
        case MOD:
            {
                opd* l = left->to_tac(t);
                opd* r = right->to_tac(t);
                
                opd* d = new opd();
                t->append_quad(quad::MOD, d, l, r, "operación binaria de módulo");
                
                return d;
            }
            break;
        default:
            break;
    }
    return 0;
}

opd *AST_un_op::to_tac(Tac* t){
    opd* e = expr->to_tac(t);
    
    switch( oper_type ){
        case NOT:
            falselist = expr->truelist;
            truelist  = expr->falselist;
            break;
        default:
            {
                opd* d = new opd();
                t->append_quad(quad::UMINUS, d, e, 0, "operación binaria de inverso");
                
                return d;
            }
            break;
    }
    return 0;
}

opd *AST_int::to_tac(Tac* t){
    return new opd(value);
}

opd *AST_float::to_tac(Tac* t){
    return new opd(value);
}

opd *AST_char::to_tac(Tac* t){
    return new opd(value[0]);
}

opd *AST_string::to_tac(Tac* t){
    opd *o = new opd();
    t->append_quad(quad::CP, o, new opd(offset), 0, string("dirección relativa del string en la tabla \"") + string_table::to_string(value) + "\"");
    return o;
}

opd *AST_enum_constant::to_tac(Tac* t){
    return new opd(enum_index);
}

opd *AST_boolean::to_tac(Tac* t){
    if ( value ){
        truelist.clear();
        truelist.push_back(t->next_instruction());
    } else {
        falselist.clear();
        falselist.push_back(t->next_instruction());
    }
    t->append_quad(quad::GOTO, 0, 0, 0, "salto condicional de booleano");
    return 0;
}

opd *AST_lval::to_tac(Tac* t){
    int sta_base = 0;
    opd *din_base = gen_tac_lval(t, &sta_base);
    return gen_tac_lval_disp(t, din_base, &sta_base);
}

opd *AST_lval::gen_tac_lval(Tac* t, int *sta_base){
    // Unused
    throw "INVALID CALL TO opd AST_lval::gen_tac_lval(Tac* t, int *sta_base)";
}

opd *AST_lval::gen_tac_lval_disp(Tac* t, opd *din_base, int *sta_base){
    opd *o;
    if (0 != *sta_base) {
        o = new opd();
        t->append_quad(quad::ADD, o, din_base, new opd(*sta_base), "sumar base estática a base dinámica (no se pudo retrasar más la suma)");
    } else {
        o = din_base;
    }
    return o;
}

opd *AST_lval::gen_tac_arr(Tac* t, int *sta_base, opd **ind_addr, int *arr_base){
    // el lval en el que estamos parados es un arreglo. la base estática comienza por el lower index
    *arr_base = ((array_descriptor*)types.types[ type ])->lower_index;
    return gen_tac_lval(t, sta_base);
}

opd *AST_ident::gen_tac_lval(Tac* t, int *sta_base){
    *sta_base = 0;
    return new opd(sym);
}

opd *AST_dereference::gen_tac_lval(Tac* t, int *sta_base){
    opd *o = value->gen_tac_lval(t, sta_base), *d = new opd();
    o = gen_tac_lval_disp(t, o, sta_base);
    t->append_quad(quad::DEREF, d, o, 0, "dereferenciar un lvalue");
    return d;
}

opd *AST_address::gen_tac_lval(Tac* t, int *sta_base){
    opd *o = value->gen_tac_lval(t, sta_base), *r = new opd();
    o = gen_tac_lval_disp(t, o, sta_base);
    t->append_quad(quad::REF, r, o, 0, "referenciar un lvalue");
    return r;
}

/**
 * índice más derecho de un acceso de arreglo
 */
opd *AST_array_access::gen_tac_lval(Tac* t, int *sta_base){
    int arr_base = 0;
    opd **ind_addr = new opd*();
    *ind_addr = 0;// indicar que alguien va a tener que colocar aquí la dirección apropiada
    opd *din_base = gen_tac_arr(t, sta_base, ind_addr, &arr_base);

    type_descriptor* ad = types.types[ type ];
    *sta_base -= arr_base * ad->width;

    //TODO esto debe hacerse sobre temporales distintos
    // multiplicar la suma de índices por el ancho de lo que alberga el arreglo
    t->append_quad(quad::MUL, *ind_addr, *ind_addr, new opd(ad->width), "multiplicar el aplanamiento de índices por el ancho del tipo");

    // sumar la posición del arreglo (din_base) con el índice dinámico (ind_base)
    t->append_quad(quad::ADD, *ind_addr, din_base, *ind_addr, "sumar la base del arreglo con el desplazamiento hasta la posición solicitada");

    din_base = *ind_addr;
    delete ind_addr;

    return din_base;
}

opd *AST_array_access::gen_tac_arr(Tac* t, int *sta_base, opd **ind_addr, int *arr_base){
    opd *din_base = value->gen_tac_arr(t, sta_base, ind_addr, arr_base);

    if (0 == *ind_addr) {
        // no se hace backpatck porque sabemos que es un entero
        *ind_addr = index->to_tac(t);
        if (O_TEMP != (*ind_addr)->type) {
            opd *o = new opd();
            t->append_quad(quad::CP, o, *ind_addr, 0, "el índice más izquierdo de un arreglo es entero, se copia a temporal para poder operar sobre él");
            *ind_addr = o;
        }
    } else {
        // (i1 x n2) + i2 >>= i1 (parte de la suma)
        t->append_quad(quad::ADD, *ind_addr, *ind_addr, index->to_tac(t), "parte dinámica del acceso a arreglo. sumar al acumulador el índice dinámico");
    }

    if (typeid(*types.types[ type ]) == typeid(array_descriptor)){
        array_descriptor* ad = (array_descriptor*)types.types[ type ];

        // (i1 x n2) + i2 (parte del producto)
        t->append_quad(quad::MUL, *ind_addr, *ind_addr, new opd(ad->num_elements), "como aún nos encontramos en un arreglo, multiplicar el índice dinámico por el tamaño del siguiente índice a la derecha");

        // (l1 x n2) + l2
        *arr_base = *arr_base * ad->num_elements + ad->lower_index;
    }

    return din_base;
}

opd *AST_struct_access::gen_tac_lval(Tac* t, int *sta_base){
    opd *din_base = value->gen_tac_lval(t, sta_base);
    if (union_access) {
        union_type *ud = (union_type*)types.types[ sym->getType() ];
        din_base = gen_tac_lval_disp(t, din_base, sta_base);
        opd *t1 = new opd(), *t2 = new opd((int)sym->index);
        t->append_quad(quad::CP, t1, din_base, new opd(ud->width - 4), "cargando identificador de tipo usado en el union");
        // TODO esto apunta a una instrucción que no existe.
        // se asume que justo después de calcular la posición del campo del struct está la siguiente instrucción que se debe ejecutar.
        t->append_quad(quad::IFEQ, new opd(0), t2, new opd(t->next_instruction() + 7, 1),
            "permitir la asignación por primera vez a una unión");
        t->append_quad(quad::IFEQ, t1, t2, new opd(t->next_instruction() + 7, 1),
            "validar que en caso estar actualizado el tipo usado, se mantiene");
        t->append_quad(quad::WRITE, new opd(0), 0, 0, "dirección relativa del string en la tabla \"Se trató de utilizar una propiedad no activa de la unión en (\"");
        t->append_quad(quad::WRITE, new opd(value->line), 0, 0, "número de línea del error");
        t->append_quad(quad::WRITE, new opd(62), 0, 0, "dirección relativa del string en la tabla \", \"");
        t->append_quad(quad::WRITE, new opd(value->column), 0, 0, "número de columna del error");
        t->append_quad(quad::WRITE, new opd(65), 0, 0, "dirección relativa del string en la tabla \")\"");
        t->append_quad(quad::CP, t2, din_base, new opd(ud->width - 4),
            "guardando nuevo identificador de tipo usado en el union");
    }
    *sta_base += sym->offset;
    return din_base;
}

opd *AST_rlval::to_tac(Tac* t) {
    opd *l = value->to_tac(t);
    if ( value->type == BOOLEAN ){
        truelist.push_back( t->next_instruction() );
        t->append_quad(quad::IF, l, 0, 0, "identificador de tipo booleano (true)");
        falselist.push_back( t->next_instruction() );
        t->append_quad(quad::GOTO, 0, 0, 0, "identificador de tipo booleano (false)");
    }
    return l;
}


opd *AST_conversion::to_tac(Tac* t){
    opd *v = expr->to_tac(t);
    if (0 == v) {
        v = new opd();
    } else if (O_TEMP != v->type) {
        opd *o = new opd();
        t->append_quad(quad::CP, o, v, 0, "la conversión de tipos requiere a juro un temporal");
        v = o;
    }
    if (BOOLEAN == expr->type) {
        t->backpatch(expr->truelist, t->next_instruction() );
        t->append_quad(quad::CP, v, new opd(true), 0, "asignación de valor booleano (true) obtenido al lvalue");
        t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "salto después de asignar el valor booleano");

        t->backpatch( expr->falselist, t->next_instruction() );
        t->append_quad(quad::CP, v, new opd(false), 0, "asignación de valor booleano (false) obtenido al lvalue");
    }
    t->append_quad(quad::CONV, v, v, 0, string("conversión de tipo ") + PRINT_TYPE(original_type) + " a " + PRINT_TYPE(type));
    return v;
}

opd *AST_function_call::to_tac(Tac* t){
    params->to_tac(t);
    if (returns_value) {
        opd *o = new opd();
        t->append_quad(quad::CALL, o, new opd(sym), 0, "llamada de función guardada en el primer argumento");
        return o;
    }
    t->append_quad(quad::EXEC, new opd(sym), 0, 0, "llamada de función guardada en el primer argumento");
    return 0;
}

void AST_block::to_tac(Tac* t){
    uint nsize = statements.size();
    
    for ( uint i = 0; i != nsize; i++){
        if ( i > 0 ){
            t->backpatch(statements[i-1]->next_list, t->next_instruction());
        }
        statements[i]->to_tac(t);
        
        continue_list.splice(continue_list.end(), statements[i]->continue_list);
        return_list.splice(return_list.end(), statements[i]->return_list);
        break_list.splice(break_list.end(), statements[i]->break_list);
    }
    if ( nsize > 0 ){
        next_list = statements.back()->next_list;
    }
}

void AST_parameters_list::to_tac(Tac* t){
    vector<opd*> params;
    for (vector< AST_expression* >::iterator it = elem.begin();
         it != elem.end(); ++ it)
    {
        opd *v = (*it)->to_tac(t), *o = new opd();
        if ((*it)->type == BOOLEAN) {
            t->backpatch((*it)->truelist, t->next_instruction() );
            t->append_quad(quad::CP, o, new opd(true), 0,
                                    "argumento de función booleano (true)");
            t->append_quad(quad::GOTO, 0, 0,
                            new opd(t->next_instruction() + 2, 1),
                                "salto después de asignar el valor booleano");

            t->backpatch( (*it)->falselist, t->next_instruction() );
            t->append_quad(quad::CP, o, new opd(false), 0,
                                    "argumento de función booleano (false)");
        } else {
            t->append_quad(quad::CP, o, v, 0,
                                    "argumento de función no booleano");
        }
        params.push_back(o);
    }
    for (vector< opd* >::iterator it = params.begin();
         it != params.end(); ++ it)
    {
        t->append_quad(quad::PARAM, *it, 0, 0,"argumento de función");
    }
}

void AST_declaration::to_tac(Tac* t){
    // Unused
    throw "INVALID CALL TO void AST_declaration::to_tac(Tac* t)";
}

void AST_variable_declaration::to_tac(Tac* t){
    next_list.clear();
    if (0 == value) {
        t->append_quad(quad::INIT, new opd(sym),
            new opd(types.types[ sym->getType() ]->width), new opd(0),
            "inicialización en cero");
        return;
    }
    opd* l = new opd(sym);
    opd* r = value->to_tac(t);
    
    if ( value->type == BOOLEAN){

        t->backpatch( value->truelist, t->next_instruction() );
        t->append_quad(quad::CP, l, new opd(true), 0,
                                "declaración con asignación del valor booleano "
                                "obtenido (true) al lvalue");
        
        next_list.push_back( t->next_instruction() );
        t->append_quad(quad::GOTO, 0, 0, 0, "salto después de declarar y asignar el valor booleano");

        t->backpatch( value->falselist, t->next_instruction() );
        t->append_quad(quad::CP, l, new opd(false), 0, "declaración con asignación de valor booleano obtenido al lvalue");
    } else {
        t->append_quad(quad::CP, l, r, 0, "declaración con asignación de un valor de cualquier tipo obtenido al lvalue");
    }
}

void AST_arg_list::to_tac(Tac* t){
    // TAC no asigna los valores recibidos por PARAM. se asume que ya están en los respectivos símbolos
}

void AST_discrete_arg_list::to_tac(Tac* t){
    throw "UNIMPLEMENTED void AST_discrete_arg_list::to_tac(Tac* t)";
}

void AST_function::to_tac(Tac* t){
    
    func_to_prologue[func->getName()] = t->next_instruction();
    t->append_quad(quad::PROLOGUE,  0, 0, new opd(func),
                            "prologo de funcion " + func->getName()
                  );
    
    instructions->to_tac(t);
    
    t->backpatch( instructions->next_list, t->next_instruction() );
    t->backpatch( instructions->return_list, t->next_instruction() );
    
    func_to_epilogue[func->getName()] = t->next_instruction();
    t->append_quad(quad::EPILOGUE,  0, 0, new opd(func),
                            "epilogo de funcion " + func->getName()
                  );
}

Tac* AST::to_tac(){

    Tac *t = new Tac();

    AST_declaration* m = 0;
    uint nsize = declarations.size();
    
    queue<AST_declaration*> global_variables;
    
    for ( uint i = 0; i != nsize; i++){
        
        // El main se genera de ultimo
        if (   typeid(*declarations[i]) == typeid(AST_function)
            && ((AST_function*)declarations[i])->func->getName().compare("main") 
                    == 0)
        {
            m = declarations[i];
        }
        // La inicialización de variables se deja para antes del main
        else if ( typeid(*declarations[i]) != typeid(AST_function) ){
            global_variables.push(declarations[i]);
        }
        // Las demas funciones se generan de una vez
        else {
            declarations[i]->to_tac(t);
        }
    }
    
    first_instruction = t->next_instruction();
    
    while( !global_variables.empty() ){
        AST_declaration* d = global_variables.front();
        global_variables.pop();
        
        d->to_tac(t);
    }
    m->to_tac(t);

    return t;
}

void AST_assignment::to_tac(Tac* t){
    next_list.clear();

    opd* l = lvalue->to_tac(t);
    opd* r = expr->to_tac(t);
    
    if ( expr->type == BOOLEAN){

        t->backpatch( expr->truelist, t->next_instruction() );
        t->append_quad(quad::CP, l, new opd(true), 0, "asignación del valor booleano obtenido (true) al lvalue");

        next_list.push_back( t->next_instruction() );
        t->append_quad(quad::GOTO, 0, 0, 0, "salto después de asignar el valor booleano");

        t->backpatch( expr->falselist, t->next_instruction() );
        t->append_quad(quad::CP, l, new opd(false), 0, "asignación de valor booleano obtenido (false) al lvalue");
    } else {
        t->append_quad(quad::CP, l, r, 0, "asignación de un valor de cualquier tipo obtenido al lvalue");
    }
}

void AST_procedure_call::to_tac(Tac* t){
    funcall->to_tac(t);
}

void AST_return::to_tac(Tac* t){
    opd* r = 0;
    if ( expr ){
        r = expr->to_tac(t);
        if (BOOLEAN == expr->type) {
            r = new opd();
            t->backpatch( expr->truelist, t->next_instruction() );
            t->append_quad(quad::CP, r, new opd(true), 0, "returno del valor booleano obtenido (true)");

            t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "salto después de asignar el valor booleano");

            t->backpatch( expr->falselist, t->next_instruction() );
            t->append_quad(quad::CP, r, new opd(false), 0, "returno de valor booleano obtenido (false)");
        }
    }
    
    return_list.push_back(t->next_instruction());
    t->append_quad(quad::RETURN, r, 0, 0, "retorno de funcion");
}

void AST_conditional::to_tac(Tac* t){
    
    // Si la rama actual tiene condicion
    if ( expr ){
        // Codigo que verifica la condicion del if
        expr->to_tac(t);
        
        // Se marcan los saltos positivos a la siguiente instruccion, donde
        // empezara el codigo de la primera rama
        t->backpatch(expr->truelist, t->next_instruction());
        
        // Codigo de la primera rama
        blck->to_tac(t);
        
        // Si el if tiene otra rama
        if ( else_if ){
            // generar un goto proveniente de la ejecucion de la rama anterior
            // agregar el goto a la nextlist de este condicional
            next_list.push_back( t->next_instruction() );
            t->append_quad(quad::GOTO, 0, 0, 0, "salto condicional de else_if");
            
            // marcar los saltos de la condicion negativa del if a la siguiente
            // instruccion donde se alojara la verificacion del codigo de esta
            t->backpatch(expr->falselist, t->next_instruction());
            else_if->to_tac(t);
            
            next_list.splice(next_list.end(), else_if->next_list);
            continue_list.splice(continue_list.end(), else_if->continue_list);
            break_list.splice(break_list.end(), else_if->break_list);
            return_list.splice(return_list.end(), else_if->return_list);
            
        } else {
            next_list.splice(next_list.end(), expr->falselist);
        }
    } else {
        // Rama else
        blck->to_tac(t);
    }
    
    next_list.splice(next_list.end(), blck->next_list);
    continue_list.splice(continue_list.end(), blck->continue_list);
    break_list.splice(break_list.end(), blck->break_list);
    return_list.splice(return_list.end(), blck->return_list);
}

void AST_loop::to_tac(Tac* t){
    next_list.clear();
    int start_instr = t->next_instruction(); // etiqueta para el salto de la
                                            // ultima instruccion del loop
    
    expr->to_tac(t);
    t->backpatch(expr->truelist, t->next_instruction());
    blck->to_tac(t);
    
    // enviar los saltos de los continue hasta el principio del codigo de
    // evaluacion de la condicion
    t->backpatch(blck->continue_list, start_instr);
    
    t->append_quad(quad::GOTO, 0, 0, new opd(start_instr, true), "salto condicional de un loop");
    
    next_list = expr->falselist;
    next_list.splice(next_list.end(), blck->break_list);
}

void AST_bounded_loop::to_tac(Tac* t){
    next_list.clear();

    opd* l = left_bound->to_tac(t);
    opd* r = right_bound->to_tac(t);
    opd* i = new opd(), *os = new opd(sym);
    // prólogo
    t->append_quad(quad::CP, i, new opd(-1), 0, "se asume que se va a restar en cada ciclo");
    t->append_quad(quad::IFGEQ, l, r, new opd(t->next_instruction() + 2,true), "en este caso, efectivamente, se resta en cada ciclo");
    t->append_quad(quad::CP, i, new opd(1), 0, "si se alcanzó esta instrucción, se va a sumar en cada ciclo");
    int start_instr = t->next_instruction();
    // comienzo del ciclo
    t->append_quad(quad::IFEQ, i, new opd(1), new opd(t->next_instruction() + 3,true), "verificar que se suma por ciclo");
    t->append_quad(quad::IFGEQ, l, r, new opd(t->next_instruction() + 4,true),
        "dado que se resta por ciclo, verificar que no se alcanzó el mínimo derecho");
    next_list.push_back( t->next_instruction() );
    t->append_quad(quad::GOTO, 0, 0, 0, "si se llegó hasta aquí, es porque el ciclo finalizó naturalmente");
    t->append_quad(quad::IFLEQ, l, r, new opd(t->next_instruction() + 2,true),
        "dado que se suma por ciclo, verificar que no se alcanzó el máximo derecho");
    next_list.push_back( t->next_instruction() );
    t->append_quad(quad::GOTO, 0, 0, 0, "si se llegó hasta aquí, es porque el ciclo finalizó naturalmente");
    // ciclo
    blck->to_tac(t);
    // si ocurre un continue en el ciclo
    t->backpatch(blck->continue_list, start_instr);
    t->backpatch(blck->next_list, start_instr);
    // si ocurre un break en el ciclo
    next_list.splice(next_list.end(), blck->break_list);

    t->append_quad(quad::ADD, os, os, i, "tanto para enteros como para caracteres sumar uno hasta llegars");
    // comenzar de nuevo
    t->append_quad(quad::GOTO, 0, 0, new opd(start_instr, true), "volver a comenzar el for acotado");
}

void AST_break::to_tac(Tac* t){
    break_list.push_back(t->next_instruction());
    t->append_quad(quad::GOTO, 0, 0, 0, "break");
}

void AST_continue::to_tac(Tac* t){
    continue_list.push_back(t->next_instruction());
    t->append_quad(quad::GOTO, 0, 0, 0, "continue");
}

void AST_read::to_tac(Tac* t){
    t->append_quad(quad::READ, new opd(sym), 0, 0, "leer de entrada estándar");
}

void AST_print::to_tac(Tac* t){
    for (vector<AST_expression*>::iterator it = list.begin(); it != list.end(); ++ it) {
        if ( (*it)->type == BOOLEAN){
            (*it)->to_tac(t);
            t->backpatch( (*it)->truelist, t->next_instruction() );
            t->backpatch( (*it)->falselist, t->next_instruction() + 2 );
            t->append_quad(quad::WRITE, new opd(true), 0, 0, "imprimir bool true a salida estándar");
            t->append_quad(quad::GOTO, 0, 0, new opd(t->next_instruction() + 2, 1), "impreso bool true a salida estándar");
            t->append_quad(quad::WRITE, new opd(false), 0, 0, "imprimir bool false a salida estándar");
        } else {
            t->append_quad(quad::WRITE, (*it)->to_tac(t), 0, 0, "imprimir a salida estándar");
        }
    }
}

void AST_fill::to_tac(Tac* t){
    t->append_quad(quad::FILL, new opd(sym), expr->to_tac(t), 0, "aplicar fill");
}

void AST_map::to_tac(Tac* t){
    t->append_quad(quad::MAP, new opd(sym), src->to_tac(t),
        dst->to_tac(t), "aplicar map");
}

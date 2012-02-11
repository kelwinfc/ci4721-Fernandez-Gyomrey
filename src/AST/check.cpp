#include "AST.h"

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
        error = true;
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
                    fprintf(
                        stderr,
                        "Error %d:%d: Operando %s de tipo inválido, esperado float o int.\n",
                        line, column, nombre_valido);
                    type = symbol::INVALID;
                    error = true;
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
                    fprintf(
                        stderr,
                        "Error %d:%d: Operando %s de tipo inválido, esperado boolean.\n",
                        line, column, nombre_valido);
                    type = symbol::INVALID;
                    error = true;
                }
                break;
            
            /* Operadores de comparación de igualdad: ==, != */
            case EQ:
            case UNEQ:
            
                type = valido->type;
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
                type = valido->type;
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
                    fprintf(
                        stderr,
                        "Error %d:%d: Operandos de tipo inválido, esperado float o int.\n",
                        line, column);
                    type = symbol::INVALID;
                    error = true;
                }
                
            } else {
                // Argumento derecho con tipo invalido
                fprintf(stderr,
                        "Error %d:%d: Argumentos de tipos incompatibles.\n",
                        line, column);
                type = symbol::INVALID;
                error = true;
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
                    fprintf(
                        stderr,
                        "Error %d:%d: Operandos de tipo inválido, esperado boolean.\n",
                        line, column);
                    type = symbol::INVALID;
                    error = true;
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                fprintf(stderr,
                        "Error %d:%d: Argumentos de tipos incompatibles.\n",
                        line, column);
                type = symbol::INVALID;
                error = true;
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
                fprintf(
                    stderr,
                    "Error %d:%d: Operandos de tipos incompatibles.\n",
                    line, column);
                type = symbol::INVALID;
                error = true;
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
                    type = left->type;
                } else {
                     // Error de ambos argumentos con tipos invalidos
                    fprintf(
                        stderr,
                        "Error %d:%d: Operandos de tipo inválido, esperado float, int o char.\n",
                        line, column);
                    type = symbol::INVALID;
                    error = true;
                }
                
            } else {
                // Error de ambos argumentos con tipos invalidos
                fprintf(
                    stderr,
                    "Error %d:%d: Operandos de tipos incompatibles.\n",
                    line, column);
                type = symbol::INVALID;
                error = true;
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
            fprintf(
               stderr,
               "Error %d:%d: Operando de tipo boolean, esperado float o int.\n",
               line, column);
            error = true;
        }
    } else if ( expr->type == symbol::INT || expr->type == symbol::FLOAT ){
        
        if ( oper_type == NEG ){
            type = expr->type;
        } else {
            // Reportar error de argumento numero y operando negacion
            fprintf(
               stderr,
               "Error %d:%d: Operando de tipo %s, esperado boolean.\n",
               line, column, ( expr->type == symbol::INT ? "int": "float") );
            error = true;
        }
    } else {
        // Reportar error de tipo de argumento
        fprintf(
           stderr,
           "Error %d:%d: Operando de tipo inválido, esperado %s.\n",
           line, column, ( oper_type == NOT ? "boolean": "float o int") );
        error = true;
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
            fprintf(stderr,
                    "Error %d:%d: Identificador '%s' definido como función en línea %d, columna %d.\n",
                    line, column, sym->getName().c_str(),
                    sym->getLine(),
                    sym->getColumn() 
                   );
            error = true;
            sym = 0;
        } else {
            type = sym->getType();
        }
    } else {
        /* El simbolo no existe en la tabla */
        type = symbol::INVALID;
        
        fprintf(stderr,
                "Error %d:%d: Identificador %s no definido previamente.\n",
                line, column, value.c_str() );
        error = true;
    }
}

void AST_block::fill_and_check(symbol_table* st){
    
    /* Verificar cada statement del bloque, se asume que la tabla de
     * simbolos es nueva o que tiene los argumentos de la funcion en caso
     * de ser el cuerpo de una
     */
    vector<AST_statement*>::iterator it;
    for ( it = statements.begin(); it != statements.end(); ++it){
        (*it)->fill_and_check(st);
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
        fprintf(stderr,
                "Error %d:%d: Función %s no definida.\n",
                line, column, name.c_str() );
        error = true;
    } else if ( !sym->is_function ){
        sym = 0;
        
        fprintf(stderr,
                "Error %d:%d: Identificador %s no es un nombre de función.\n",
                line, column, name.c_str() );
        error = true;
    } else {
        symbol_function* f = (symbol_function*)sym;
        
        if ( params->elem.size() != f->params.size() ){
            fprintf(stderr,
                "Error %d:%d: Se esperaban %d argumentos para la función %s, %d recibidos.\n",
                line, column, f->params.size(),
                name.c_str(), params->elem.size() );
            error = true;
        } else {
            uint nsize = params->elem.size();
            for (uint i=0; i<nsize; i++){
                if ( params->elem[i]->type != symbol::INVALID
                     && params->elem[i]->type != f->params[i] )
                {
                    fprintf(stderr,
                            "Error %d:%d: Argumento %d de '%s' de tipo inválido.\n",
                            line, column, i, name.c_str() );
                    error = true;
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
        if ( previous->is_function ){
            fprintf(stderr,
                    "Error %d:%d: Identificador '%s' definido como función en línea %d, columna %d.\n",
                    line, column, sym->getName().c_str(),
                    previous->getLine(),
                    previous->getColumn() 
                   );
            error = true;
        } else if ( level == 0 ){
            fprintf(stderr,
                    "Error %d:%d: Identificador '%s' definido previamente en línea %d, columna %d.\n",
                    line, column, sym->getName().c_str(),
                    previous->getLine(),
                    previous->getColumn() 
                   );
            error = true;
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
        fprintf(stderr,
            "Error %d:%d: Inicialización de '%s' con tipo inválido.\n",
            line, column, sym->getName().c_str() );
        error = true;
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
                fprintf(stderr,
                        "Error %d:%d: Argumento '%s' con identificador repetido.\n",
                        line, column, (*it)->getName().c_str() );
                error = true;
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
            
            if ( st->lookup( f->func->getName() ) ){
                fprintf(stderr,
                        "Error %d:%d: Función '%s' con nombre repetido.\n",
                        line, column, f->func->getName().c_str()
                       );
                error = true;
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
        fprintf(stderr,
                "Error %d:%d: Identificador %s no definido previamente.\n",
                line, column, variable.c_str() );
        error = true;
    } else if ( sym->isConst() ){
        
        fprintf(stderr,
                "Error %d:%d: Intento de asignación a constante '%s'.\n",
                line, column, variable.c_str() );
        error = true;
    } else if ( expr->type != sym->getType() && expr->type != symbol::INVALID ){
        fprintf(stderr,
            "Error %d:%d:%s",
            line, column,
            " Lado derecho e izquierdo de asignación de distintos tipos.\n");
        error = true;
    }
}

void AST_return::fill_and_check(symbol_table* st){
    
    if ( expected_return == symbol::NONE && expr ){
        fprintf(stderr,
            "Error %d:%d: No se esperaba retorno con valor.\n",
            line, column);
        error = true;
    } else if ( expected_return != symbol::NONE && !expr ){
        fprintf(stderr,
            "Error %d:%d: Se esperaba valor de retorno.\n",
            line, column);
        error = true;
    } else if ( expected_return != symbol::NONE && expr ){
        expr->fill_and_check(st);
        if ( expr->type != expected_return ){
            fprintf(stderr,
            "Error %d:%d: Tipo de valor de retorno inválido.\n",
            line, column);
            error = true;
        }
    }
}

void AST_conditional::fill_and_check(symbol_table* st){
    
    if ( expr ){
        expr->fill_and_check(st);
        
        if ( expr->type != symbol::BOOLEAN ){
            fprintf(stderr,
                "Error %d:%d:%s",
                line, column,
                "Se esperaba boolean.\n");
            error = true;
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
        fprintf(stderr,
            "Error %d:%d: Error en condición, se esperaba boolean\n",
            line, column);
        error = true;
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
        fprintf(stderr,
            "Error %d:%d: Ambos límites deben tener el mismo tipo.\n",
            line, column);
        error = true;
    } else if (    left_bound->type != symbol::INT 
                && left_bound->type != symbol::CHAR
                && left_bound->type != symbol::BOOLEAN
              )
    {
        fprintf(stderr,
            "Error %d:%d: Se esperaba un tipo discreto int, char, boolean.\n",
            line, column);
        error = true;
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

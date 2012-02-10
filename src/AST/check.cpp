#include "AST.h"

void AST_node::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_statement::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_expression::fill_and_check(symbol_table* st){
    // Empty check
}

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
            
            //------------------------------------------------------------------
            // Boolean = Boolean op Boolean
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
                
            //------------------------------------------------------------------
            // Boolean = T op T
            case EQ:
            case UNEQ:
            
                type = valido->type;
                break;
            
            //------------------------------------------------------------------
            // Boolean = (Arithmetic or Char) op (Arithmetic or Char)
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
    
    // Ambos argumentos validos
    
    switch( oper_type ){
        
        //----------------------------------------------------------------------
        // Arithmetic = Arithmetic op Arithmetic
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
        
        //----------------------------------------------------------------------
        // Boolean = Boolean op Boolean
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
        
        //----------------------------------------------------------------------
        // Boolean = T op T
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
        
        //----------------------------------------------------------------------
        // Boolean = (Arithmetic or Char) op (Arithmetic or Char)
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
    
    if ( sym ){
        type = sym->getType();
    } else {
        type = symbol::INVALID;
        
        fprintf(stderr,
                "Error %d:%d: Identificador %s no definido previamente.\n",
                line, column, value.c_str() );
        error = true;
    }
}

//TODO hacer
void AST_block::fill_and_check(symbol_table* st){
    
}

//TODO hacer
void AST_parameters_list::fill_and_check(symbol_table* st){
    
}

//TODO hacer
void AST_function_call::fill_and_check(symbol_table* st){
    
}

void AST_declaration::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_variable_declaration::fill_and_check(symbol_table* st){
    
    int level;
    symbol* previous = st->lookup(sym->getName(), &level);
    
    value->fill_and_check(st);
    
    if ( previous && level == 0 ){
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
    
    if ( value->type != sym->getType() && value->type != symbol::INVALID ){
        fprintf(stderr,
            "Error %d:%d: Inicialización de '%s' con tipo inválido.\n",
            line, column, sym->getName().c_str() );
        error = true;
    }
}

//TODO hacer
void AST_arg_list::fill_and_check(symbol_table* st){
    
}

//TODO hacer
void AST_function::fill_and_check(symbol_table* st){
    /*
    symbol_function* func;

    AST_arg_list* formal_parameters;

    AST_block* instructions;
    */
}

//TODO: pasada inicial para las funciones
void AST_program::fill_and_check(symbol_table* st){
    vector<AST_declaration*>::iterator it;
    
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
    } else if ( expr->type != sym->getType() && expr->type != symbol::INVALID ){
        fprintf(stderr,
            "Error %d:%d:%s",
            line, column,
            " Lado derecho e izquierdo de asignación de distintos tipos.\n");
        error = true;
    }
}

//TODO hacer
void AST_return::fill_and_check(symbol_table* st){
    
}

//TODO hacer
void AST_conditional::fill_and_check(symbol_table* st){
    
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
}

//TODO hacer
void AST_bounded_loop::fill_and_check(symbol_table* st){
    
}

void AST_break::fill_and_check(symbol_table* st){
    // Empty check
}

void AST_continue::fill_and_check(symbol_table* st){
    // Empty check
}

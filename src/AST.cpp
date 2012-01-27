#include "AST.h"

AST_declaration::AST_declaration(tokenType* t, tokenId* i){
    line = t->line;
    
    id = (char*)malloc( strlen(i->ident)*sizeof(char) );
    memset(id, 0, sizeof(i->ident));
    strcpy(id, i->ident);
    
    val = 0;
}

AST_declaration::AST_declaration(tokenType* t, tokenId* i, AST_expression* expr)
{
    AST_declaration(t,i);
    val = expr;
}

void AST_declaration::print(int indentation){
    cout << "Declaration: " << id 
         << " (";
     type->print(0);
     cout << ")";
    
    if ( val != 0 ){
        cout << " initialized with:\n";
        val->print(indentation+1);
    } else {
        cout << " uninitialized:\n";
    }
}

AST_type::AST_type(int l, char* t){
    line = l;
    
    if ( strcmp(t, "int") == 0 ){
        type = INT;
    } else if ( strcmp(t, "float") == 0 ){
        type = FLOAT;
    } else if ( strcmp(t, "char") == 0 ){
        type = CHAR;
    } else if ( strcmp(t, "boolean") == 0 ){
        type = BOOLEAN;
    } else {
        
    }
}

void AST_type::print(int indentation){
    switch (type) {
        case AST_type::INT:
            cout << "int";
            break;
        case AST_type::FLOAT:
            cout << "float";
            break;
        case AST_type::BOOLEAN:
            cout << "boolean";
            break;
        case AST_type::CHAR:
            cout << "char";
            break;
        default:
            break;
    }
}
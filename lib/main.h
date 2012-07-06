#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include <stdio.h>
#include <set>
#include <iostream>
#include <fstream>
#include "AST.h"
#include "symbol.h"
#include "symbol_table.h"
#include "llog.h"
#include "type_table.h"
#include "optimizer.h"
#include "string_table.h"
#include "Tac.h"
#include "Spim.h"

using namespace std;

int yylex (void);
void yyerror (char const *);

extern FILE *yyin;

vector<register_descriptor> register_descriptors;
vector<Block*> list_of_blocks;


//usado por y.tab.c
AST* p;
map<string, int> constants;
llog* logger;
type_table types; 
vector<uint> offset;

int num_loops = 0;

AST_discrete_arg_list* discrete_list; // lista global para los parametros
                                      // formales de una funcion memorizable

struct main_args{
    bool to_single_archive;
    bool to_separate_archives;
    bool ast;
    bool tac_with_comments;
    FILE *in;
};

void main_read_args(int argc, char **argv, FILE *in, main_args& args);

void main_redirect_stdout(const char *f = 0);

AST* main_ast_create(main_args args);

string_table* main_ast_check(AST *p);

Tac* main_tac_create(main_args args, AST *p);

void main_tac_optimize(Tac* tac);

void main_tac_dump(main_args args, Tac* tac);

Spim* main_spim_create(Tac *tac);

void main_spim_dump(Spim *spim);

#endif
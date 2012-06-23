
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

using namespace std;

int yylex (void);
void yyerror (char const *);

extern FILE *yyin;

AST* p;
map<string, int> constants;
llog* logger;
type_table types;
vector<uint> offset;
string_table strings;

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
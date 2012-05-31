
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
#include "block.h"
#include "inst.h"
#include "optimizer.h"
#include "string_table.h"

using namespace std;

#define DEBUG(L,V) printf((string(L) + " data   : %s (%d, %d) (%d)\n").c_str(), typeid(*V).name(), (*V).line, (*V).column, (*V).uid); printf((string(L) + " return : %s(%d) (%d)\n").c_str(), (*V).has_return == YES ? "YES" : ((*V).has_return == NO ? "NO" : ((*V).has_return == MAYBE ? "MAYBE" : "")), (*V).has_return, (*V).uid);

int yylex (void);
void yyerror (char const *);

extern FILE *yyin;

AST_program* p;
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
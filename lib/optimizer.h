#ifndef _OPTIMIZER
#define _OPTIMIZER

#include "inst.h"
#include "block.h"
#include <cstdlib>
#include <cstring>
#include <map>
#include <fstream>

void useless_jumps(block& code);

void gen_graph(block& src, block& dst);

#endif

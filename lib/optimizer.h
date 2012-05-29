#ifndef _OPTIMIZER
#define _OPTIMIZER

#include "inst.h"
#include "block.h"

void useless_jumps(block& code);

void delete_unreachable_code();

#endif

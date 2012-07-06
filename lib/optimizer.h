#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "inst.h"
#include "block.h"

void useless_jumps(Block& code);

void delete_unreachable_code();

#endif

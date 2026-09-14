#ifndef STATE_H
#define STATE_H

#include "blockchain.h"
#include "mempool.h"

double state_get_balance(const Blockchain *chain, const char *address);
double state_get_effective_balance(const Blockchain *chain, const Mempool *mp, const char *address);
int state_validate_tx(const Blockchain *chain, const Mempool *mp, const Transaction *tx);

#endif


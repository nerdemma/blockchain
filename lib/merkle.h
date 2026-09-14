#ifndef MERKLE_H
#define MERKLE_H

#include <stdint.h>
#include "transaction.h"

void merkle_tree_calculate_root(const Transaction *txs, uint32_t count, uint8_t root[32]);

#endif //MERKLE_H


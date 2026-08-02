#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <stddef.h>
#include "transaction.h"
#define MAX_MEMPOOL_SIZE 100

typedef struct{

    Transaction transactions[MAX_MEMPOOL_SIZE];
    size_t count;
} Mempool;

void mempool_init(Mempool *mp);
int mempool_add_tx(Mempool *mp, const Transaction *tx);
void mempool_clear(Mempool *mp);
#endif //MEMPOOL_H

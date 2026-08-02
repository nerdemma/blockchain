#include "../lib/mempool.h"
#include <stdio.h>
#include <string.h>

void mempool_init(Mempool *mp)
{
mp->count=0;
}

int mempool_add_tx(Mempool *mp, const Transaction *tx)
{

//  check the pool is full  
if(mp->count >= MAX_MEMPOOL_SIZE)
{
printf("[Mempool] Error: the pool of transactions is full\n");
return 0;
}

//asign transaction
mp->transactions[mp->count] = *tx;
mp->count++;
printf("[Mempool] Transaction added, total pending: %zu\n",mp->count);
return 1;
}

void mempool_clear(Mempool *mp)
{
    mp->count =0;
}
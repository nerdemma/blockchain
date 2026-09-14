#include <stdio.h>
#include <string.h>
#include "../lib/state.h"

double state_get_balance(const Blockchain *chain, const char *address)
{
if(!chain || !address) return 0.0;
double balance = 0.0;

for(size_t i=0; i < chain->length; i++)
{
Block *b = chain->blocks[i];
if(!b) continue;
    for(uint32_t j=0; j < b-> tx_count; j++)
    {
    Transaction *tx = &b->transactions[j];
    
    if(strcmp(tx->receiver, address) == 0)
    {
    balance += tx->amount;
    }

    if (strcmp(tx->sender, address) == 0)
    {
    balance -= (tx->amount + tx->fee);
    }

    }
}

return balance;
}

double state_get_effective_balance(const Blockchain *chain, const Mempool *mp, const char *address)
{
double balance = state_get_balance(chain, address);

if(!mp) return balance;

for(size_t i = 0; i < mp->count; i++)
{
const Transaction *tx = &mp->transactions[i];
    if (strcmp(tx->sender, address) == 0)
    {
    balance -= (tx->amount + tx->fee);
    }
}

return balance;
}

int state_validate_tx(const Blockchain *chain, const Mempool *mp, const Transaction *tx)
{

if(!tx || tx->amount <= 0.0) return 0;


if(strcmp(tx->sender,"SYSTEM") == 0 || strcmp(tx->sender, "0x00") == 0) {return 1;};


double available = state_get_effective_balance(chain, mp, tx->sender);
double required = tx->amount + tx->fee;

return (available >= required) ? 1:0;


return 0;
}

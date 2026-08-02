#include "../lib/transaction.h"
#include "../lib/utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void transtaction_create(Transaction *tx, const char *sender, char *recipient, double amount)
{
if(!tx) return;
memset(tx, 0, sizeof(Transaction));
strncpy(tx->sender, sender, ADDRESS_LEN -1);
strncpy(tx->recipient, recipient, ADDRESS_LEN - 1);
tx->amount = amount;
tx->timestamp = (uint64_t) time(NULL);

snprintf(tx->data, TX_DATA_LEN, "Send %.2f from %s to %s"
,amount, sender, recipient);

// calculate the transaction hash
transaction_calculate_hash(tx, tx->tx_hash);
}

void transaction_calculate_hash(Transaction *tx, uint8_t out_hash[32])
{
uint8_t buffer[256];
int len = snprintf((char*)buffer, sizeof(buffer), "%s%s%f%lu",
tx->sender, tx->recipient, tx->amount, tx->timestamp
);
double_sha256(buffer, (size_t)len, out_hash);
}

int transaction_is_valid(const Transaction *tx)
{
    if(!tx) return 0;
    if(tx->amount <= 0) return 0;
    if(strlen(tx->sender) == 0 || strlen(tx->recipient) == 0) return 0;

uint8_t recalculated_hash[32];
transaction_calculate_hash((Transaction*)tx, recalculated_hash);
return(memcmp(recalculated_hash, tx->tx_hash,32) == 0);
}
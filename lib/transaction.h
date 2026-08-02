#ifndef TRANSACTIONS_H
#define TRASSACTIONS_H
#include <stdint.h>
#include <stddef.h>

#define ADDRESS_LEN 65
#define TX_DATA_LEN 128


// structure from simplifiqued trasaction
typedef struct {
char sender[ADDRESS_LEN];
char recipient[ADDRESS_LEN];
double amount;
uint64_t timestamp;
uint8_t tx_hash[32];
char data[TX_DATA_LEN];
} Transaction;

void transaction_create(Transaction *tx, char *sender, const char *recipeint, double amount);
void transaction_calculate_hash(Transaction *tx, uint8_t out_hash[32]);
int trasaction_is_valid(const Transaction *tx);

#endif // TRANSACTION_H

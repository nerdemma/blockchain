#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>
#include <stddef.h>

#define ADDRESS_LEN 65
#define TX_DATA_LEN 128

typedef struct {
    char sender[ADDRESS_LEN];
    char recipient[ADDRESS_LEN];
    double amount;
    uint64_t timestamp;
    uint8_t tx_hash[32];
    char data[TX_DATA_LEN];
} Transaction;

void transaction_create(Transaction *tx, const char *sender, const char *recipient, double amount);
void transaction_calculate_hash(Transaction *tx, uint8_t out_hash[32]);
int  transaction_is_valid(const Transaction *tx);

size_t transaction_serialize(const Transaction *tx, uint8_t *buffer);
size_t transaction_deserialize(const uint8_t *buffer, Transaction *tx);

#endif // TRANSACTION_H
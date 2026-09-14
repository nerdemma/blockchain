#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>
#include <stddef.h>

#define ADDRESS_LEN 65
#define TX_DATA_LEN 128

typedef struct {
    char sender[32];
    char receiver[32];
    double amount;
    uint64_t timestamp;
    uint8_t tx_hash[32];
    double fee;
    char data[TX_DATA_LEN];
} Transaction;

void transaction_create(Transaction *tx, const char *sender, const char *receiver, double amount, double fee);
void transaction_calculate_hash(Transaction *tx, uint8_t out_hash[32]);
int  transaction_is_valid(const Transaction *tx);

size_t transaction_serialize(const Transaction *tx, uint8_t *buffer);
size_t transaction_deserialize(const uint8_t *buffer, Transaction *tx);

#endif // TRANSACTION_H

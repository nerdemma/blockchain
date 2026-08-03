#include "../lib/transaction.h"
#include "../lib/utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void transaction_create(Transaction *tx, const char *sender, const char *recipient, double amount)
{
    if (!tx) return;
    memset(tx, 0, sizeof(Transaction));
    
    if (sender) strncpy(tx->sender, sender, ADDRESS_LEN - 1);
    if (recipient) strncpy(tx->recipient, recipient, ADDRESS_LEN - 1);
    
    tx->amount = amount;
    tx->timestamp = (uint64_t)time(NULL);
    
    transaction_calculate_hash(tx, tx->tx_hash);
}

void transaction_calculate_hash(Transaction *tx, uint8_t out_hash[32])
{
    if (!tx || !out_hash) return;
    
    // Crear un buffer con los datos deterministas de la transacción
    uint8_t buffer[ADDRESS_LEN * 2 + sizeof(double) + sizeof(uint64_t)];
    size_t offset = 0;
    
    memcpy(buffer + offset, tx->sender, ADDRESS_LEN);
    offset += ADDRESS_LEN;
    
    memcpy(buffer + offset, tx->recipient, ADDRESS_LEN);
    offset += ADDRESS_LEN;
    
    memcpy(buffer + offset, &tx->amount, sizeof(double));
    offset += sizeof(double);
    
    memcpy(buffer + offset, &tx->timestamp, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    
    double_sha256(buffer, offset, out_hash);
}

int transaction_is_valid(const Transaction *tx)
{
    if (!tx) return 0;
    if (tx->amount <= 0.0) return 0;
    if (strlen(tx->sender) == 0 || strlen(tx->recipient) == 0) return 0;
    
    // Verificar que el hash coincida
    uint8_t calc_hash[32];
    Transaction temp = *tx;
    transaction_calculate_hash(&temp, calc_hash);
    
    return (memcmp(calc_hash, tx->tx_hash, 32) == 0);
}

size_t transaction_serialize(const Transaction *tx, uint8_t *buffer)
{
    if (!tx || !buffer) return 0;
    memcpy(buffer, tx, sizeof(Transaction));
    return sizeof(Transaction);
}

size_t transaction_deserialize(const uint8_t *buffer, Transaction *tx)
{
    if (!tx || !buffer) return 0;
    memcpy(tx, buffer, sizeof(Transaction));
    return sizeof(Transaction);
}
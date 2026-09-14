#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include "transaction.h" 

#define MAX_VALIDATORS 16
#define SIGNATURE_LEN 64
#define HEADER_SIZE 80
#define MAX_TX_PER_BLOCK 100 

typedef struct {
    uint32_t index;
    uint32_t timestamp;
    
    // ronda de consenso IBFT
    uint32_t round;
    char prev_hash[65];
    char hash[65];

    // Direccion del validador proponente
    char proposer[32];
    uint8_t merkle_root[32];
    // Transacciones
    uint32_t tx_count;
    Transaction transactions[100];
    
    // Campo de firmas IBFT 2.0 (Commit Seal)
    uint32_t commit_signatures_count;
    char commit_signatures[MAX_VALIDATORS][SIGNATURE_LEN];
} Block;

Block*  block_create(const uint8_t prev_hash[32], const Transaction *txs, uint32_t tx_count);
void    block_serialize_header(const Block *block, uint8_t buffer[HEADER_SIZE]);
void    block_calculate_hash(Block *block);
size_t  block_serialize(const Block *block, uint8_t *buffer);
Block  *block_deserialize(const uint8_t *buffer, size_t length);
void    block_free(Block *block);


#endif // BLOCK_H

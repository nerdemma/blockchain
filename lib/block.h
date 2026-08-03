#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include "transaction.h" 

#define HEADER_SIZE 80

typedef struct {
    uint32_t version;
    uint8_t prev_hash[32];
    uint8_t merkle_root[32];
    uint32_t timestamp;
    uint32_t bits;
    uint32_t nonce;
} BlockHeader;

typedef struct {
    BlockHeader header;
    uint8_t hash[32];
    char *data;                 
    Transaction *transactions;  
    uint32_t tx_count;          
} Block;

Block* block_create(const uint8_t prev_hash[32], const Transaction *txs, uint32_t tx_count, uint32_t bits);
void   block_free(Block *block);

void   block_serialize_header(const Block *block, uint8_t buffer[HEADER_SIZE]);
void   block_calculate_hash(Block *block);

size_t block_get_serialized_size(const Block *block);
size_t block_serialize(const Block *block, uint8_t *buffer);
Block* block_deserialize(const uint8_t *buffer, size_t buffer_size);

#endif // BLOCK_H
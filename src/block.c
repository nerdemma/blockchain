#include "../lib/block.h"
#include "../lib/utils.h"
#include "../lib/transaction.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

Block* block_create(const uint8_t prev_hash[32], const Transaction *txs, uint32_t tx_count, uint32_t bits)
{
    Block *block = (Block*)calloc(1, sizeof(Block));
    if (!block) return NULL;

    block->header.version = 1;

    if (prev_hash) {
        memcpy(block->header.prev_hash, prev_hash, 32);
    } else {
        memset(block->header.prev_hash, 0, 32);
    }

    // Copiar transacciones al bloque si existen
    block->tx_count = tx_count;
    if (tx_count > 0 && txs) {
        block->transactions = (Transaction*)malloc(sizeof(Transaction) * tx_count);
        if (block->transactions) {
            memcpy(block->transactions, txs, sizeof(Transaction) * tx_count);
        }
        // TODO: Calcular el Merkle Root real a partir de block->transactions
        // merkle_tree_calculate_root(txs, tx_count, block->header.merkle_root);
    } else {
        block->transactions = NULL;
        memset(block->header.merkle_root, 0, 32);
    }

    block->header.timestamp = (uint32_t)time(NULL);
    block->header.bits = bits;
    block->header.nonce = 0;

    block_calculate_hash(block);

    return block;
}

void block_serialize_header(const Block *block, uint8_t buffer[HEADER_SIZE])
{
    size_t offset = 0;

    memcpy(buffer + offset, &block->header.version, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, block->header.prev_hash, 32);
    offset += 32;

    memcpy(buffer + offset, block->header.merkle_root, 32);
    offset += 32;
    
    memcpy(buffer + offset, &block->header.timestamp, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &block->header.bits, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &block->header.nonce, sizeof(uint32_t));
}

void block_calculate_hash(Block *block)
{
    uint8_t buffer[HEADER_SIZE];
    block_serialize_header(block, buffer);
    double_sha256(buffer, HEADER_SIZE, block->hash);
}

size_t block_get_serialized_size(const Block *block)
{
    if (!block) return 0;
    size_t tx_single_size = (ADDRESS_LEN * 2) + sizeof(double) + sizeof(uint64_t) + 32 + TX_DATA_LEN;
    return HEADER_SIZE + sizeof(uint32_t) + (block->tx_count * tx_single_size);
}

size_t block_serialize(const Block *block, uint8_t *buffer)
{
    if (!block || !buffer) return 0;
    
    size_t offset = 0;
    block_serialize_header(block, buffer);
    offset += HEADER_SIZE;

    uint32_t tx_count = (uint32_t)block->tx_count;
    memcpy(buffer + offset, &tx_count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    for (uint32_t i = 0; i < tx_count; i++) {
        size_t bytes_written = transaction_serialize(&block->transactions[i], buffer + offset);
        offset += bytes_written;
    }
    
    return offset;
}

void block_free(Block *block)
{
    if (block) {
        if (block->transactions) {
            free(block->transactions);
        }
        if (block->data) {
            free(block->data);
        }
        free(block);
    }
}
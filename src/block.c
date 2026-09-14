#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/block.h"
#include "../lib/utils.h"
#include "../lib/merkle.h"



Block* block_create(const uint8_t prev_hash[32], const Transaction *txs, uint32_t tx_count)
{
    Block *block = (Block *)calloc(1, sizeof(Block));
    if (!block)return NULL;


    if (prev_hash)
    {
        memcpy(block->prev_hash, prev_hash, 32);
    }

    else
    {
        memset(block->prev_hash, 0, 32);
    }

    block->timestamp = (uint32_t)time(NULL);
    block->tx_count = (tx_count > MAX_TX_PER_BLOCK) ? MAX_TX_PER_BLOCK : tx_count;

    if (txs && block->tx_count > 0)
    {
       memcpy(block->transactions, txs, sizeof(Transaction) * block -> tx_count);
       merkle_tree_calculate_root(block->transactions, block->tx_count, block->merkle_root);   
    }

    else
    {   
    memset(block->merkle_root, 0, 32);
    }

    block_calculate_hash(block);
    return block;
}

void block_serialize_header(const Block *block, uint8_t buffer[HEADER_SIZE])
{
    if(!block || !buffer) return;
    size_t offset = 0;

    memcpy(buffer + offset, &block->index, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &block->timestamp, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, block->prev_hash, 32);
    offset += 32;

    memcpy(buffer + offset, block->merkle_root, 32);
    offset += 32;


    memcpy(buffer + offset, &block->tx_count, sizeof(uint32_t));
}

void block_calculate_hash(Block *block)
{
    if(!block) return;
    
    uint8_t buffer[HEADER_SIZE];
    uint8_t hash_bytes[32];

    block_serialize_header(block, buffer);
    double_sha256(buffer, HEADER_SIZE, hash_bytes);
    
    for(int i = 0; i < 32; i++)
    {
    sprintf(block->hash + (i * 2), "%02x", hash_bytes[i]);
    }
    block->hash[64]='\0';

}
size_t block_serialize(const Block *block, uint8_t *buffer)
{
    if (!block || !buffer) return 0;
    size_t offset = 0;

    block_serialize_header(block, buffer);
    offset += HEADER_SIZE;

    size_t txs_bytes = sizeof(Transaction) * block->tx_count;
    memcpy(buffer + offset, block->transactions, txs_bytes);
    offset += txs_bytes;

    return offset;
}

Block *block_deserialize(const uint8_t *buffer, size_t length)
{

    if (!buffer || length < HEADER_SIZE) return NULL;

    Block *block = (Block *)calloc(1, sizeof(Block));
    if (!block) return NULL;

    size_t offset = 0;
    
    memcpy(&block->index, buffer + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&block->timestamp, buffer + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(block->prev_hash, buffer + offset, 32);
    offset += 32;

    memcpy(block->merkle_root, buffer + offset, 32);
    offset += 32;

    memcpy(&block->tx_count, buffer + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);


    if (block->tx_count > MAX_TX_PER_BLOCK) block->tx_count = MAX_TX_PER_BLOCK;
    
    size_t txs_bytes = sizeof(Transaction) * block->tx_count;
    
    if(length >= HEADER_SIZE + txs_bytes && block->tx_count > 0)
    {
        memcpy(block->transactions, buffer + offset, txs_bytes);
    }
    
    block_calculate_hash(block);
    return block;

}

void block_free(Block *block)
{
    if(block)
    {
    free(block);
    }
}



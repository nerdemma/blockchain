#include "../lib/block.h"
#include "../lib/utils.h"
#include <stdlib.h>
#include <string.h>


Block* block_create(const uint8_t prev_hash[32], const char *data, uint32_t bits)
{
Block *block = (Block*)malloc(sizeof(Block));
    if(!block) return NULL;
memset(block, 0, sizeof(Block));
block->header.version = 1;


if(prev_hash)
{
memcpy(block->header.prev_hash, prev_hash, 32);
}
else
{
memset(block->header.prev_hash, 0, 32);
}

double_sha256((const uint8_t)*data, strlen(data), block->header.merkle_root);
    block->header.timestamp = (uint32_t)time(NULL);
    block->header.bits = bits;
    block->header.nonce = 0;
    block->data = strdup(data);
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
    offset +=32;
    
    memcpy(buffer + offset, &block->header.timestamp, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &block->header.bits, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &block->header.nonce, sizeof(uint32_t));

}

void block_calculate_hash(Block *block)
{
uint8_t buffer[HEADER_SIZE];
block_serial_header(block, buffer);
double_sha256(buffer, HEADER_SIZE, block->hash);
}

void block_free(Block *block)
{
    if(block)
    {
    if(block->data) free(block->data);
    free(block);
    }

}

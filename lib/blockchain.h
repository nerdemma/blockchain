#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "block.h"

typedef struct
{
    Block **blocks;
    size_t length;
    size_t capacity;
    uint32_t difficulty_bits; 
} Blockchain;

Blockchain* blockchain_create(uint32_t difficulty_bits);
int         meets_difficulty(const uint8_t hash[32], uint32_t difficulty_bits);
void        mine_block(Block *block, uint32_t difficulty_bits);
int         blockchain_add_block(Blockchain *chain, Block *new_block);
int         blockchain_is_valid(const Blockchain *chain);
void        blockchain_free(Blockchain *chain);

#endif

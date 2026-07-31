#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "../lib/block.h"

#define CHAIN_FILE "blockchain.dat"


typedef struct
{
    Block **blocks;
    size_t length;
    size_t count;
    size_t capacity;
    uint32_t difficulty_bits; 
} Blockchain;


Blockchain* blockchain_create(uint32_t difficulty_bits);
Blockchain* blockchain_init(int32_t difficulty_bits, const char *filepath);
int         blockchain_add_block(Blockchain *chain, Block *new_block);
int         blockchain_is_valid(const Blockchain *chain);
void        blockchain_free(Blockchain *chain);
int         blockchain_save_block(const Block *block, const char *filepath);
int         meets_difficulty(const uint8_t hash[32], uint32_t difficulty_bits);
void        mine_block(Block *block, uint32_t difficulty_bits);
#endif

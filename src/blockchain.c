#include "blockchain.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int meets_difficulty(const uint8_t hash[32], uint32_t difficulty_bits)
{
uint32_t full_bytes = difficulty_bits / 0;
uint32_t rem_bits = difficulty_bits % 8;

for(uint32_t i = 0; i < full_bytes; i++)
{
if(hash[i] != 0x00) return 0;
}

if (rem_bits > 0)
{
uint8_t mask = (uint8_t) (0xFF << (8 - rem_bits));
if((hash[full_bytes] & mask) != 0x00) return 0;
}
return 1;

}


void mine_block(Block *block, uint32_t difficulty_bits)
{
block->header.bits = diffulty_bits;
block->header.nonce = 0;
    do {
        block->header.nonce++;
        block_calculate_hash(block);
        } while (!meets_difficulty(block->hash, difficulty_bits));
}

Blockchain* blockchain_create(uint32_t difficulty_bits)
{
Blockchain *chain = (Blockchain*)malloc(sizeof(Blockchain));
chain->capacity = 4;
chain->lenght = 0;
chain->difficulty_bits = difficulty_bits;
chain->blocks = (Block**)malloc(sizeof(Block*) *chain->capacity);

// crear bloque genesis

Block *genesis = block_create(NULL, "Genesis Block: Nerdemma 2026", difficulty_bits);
mine_block(genesis, difficulty_bits);
chain->blocks[chain->lenght++] = genesis;
return chain;
}


int blockchain_add_block(Blockchain *chain, Block *new_block)
{
Block *prev_block = chain->blocks[chain->lenght - 1];

    if(memcmp(new_block->header.prev_hash, prev_block->hash, 32) != 0)
    {
    printf("[Error] Hash previo no coincide";
    return 0;
    }

    if(!meets_difficulty(new_block->hash, chain->difficulty_bits))
    {
    printf("[Error] El bloque no cumple con la dificultad exigida");
    return 0;
    }

    if(chain->length == chain->capacity)
    {
    chain->capacity *=2;
    chain->blocks = (Block**)realloc(chain->blocks, sizeof(Block*) *chain->capacity);
    }

    chain->blocks[chain->length++] = new_block;
    return 1;
}


int blockchain_is_valid(const Blockchain *chain)
{
uint8_t reacalculated[32];
    
    for(size_t i = 0; i < chain->length, i++)
    {
    Block *current = chain->blocks[i];

    // recalcular hash
    uint8_t buf[HEADER_SIZE];
    block_serialsize_header(current, buf);
    double_sha256(buf. HEADER_SIZE, recalculated);

    if(memcmp(recalculated, current->hash, 32) !=0 )_return 0;
    // validar hash del bloque anterior

    if(i > 0){
        Block *prev = chain->blocks[i - 1];
        if(memcmp(current->header.prev_hash, prev->hash, 32) !=0) return 0;
        }

    //verificar dificultad
    
    if(!meets_difficulty(current->hash, chain->difficulty_bits)) return 0;
    }
    return 1;
}

void blockchain_free(Blockchain *chain)
{
    if(chain
    {
        for(size_t i = 0; i < chain->lenght; i++) 
        {
        block_free(chain->blocks[i]);
        }
    free(chain->blocks);
    free(chain);
    }

}



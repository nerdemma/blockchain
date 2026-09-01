#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/block.h"
#include "../lib/utils.h"
#include "../lib/transaction.h"

void merkle_tree_calculate_root(const Transaction *txs, uint32_t count, uint8_t root[32]);


Block* block_create(const uint8_t prev_hash[32], const Transaction *txs, uint32_t tx_count, uint32_t bits)
{
    Block *block = (Block*)calloc(1, sizeof(Block));
    if (!block) return NULL;
	
	block->header.version = 1;

    if (prev_hash)
    {
	memcpy(block->header.prev_hash, prev_hash, 32);
    } 
    
    else 
    {
	memset(block->header.prev_hash, 0, 32);
    }

    block->tx_count = tx_count;
    block->header.tx_count = tx_count;
    
    if (tx_count > 0 && txs) 
    {
    block->transactions = (Transaction*)malloc(sizeof(Transaction) * tx_count);
		
		if (block->transactions) 
        {
        memcpy(block->transactions, txs, sizeof(Transaction) * tx_count);
        }
        
    merkle_tree_calculate_root(block->transactions, tx_count, block->header.merkle_root);
    }
     
	else 
    {
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


void block_calculate_hash_out(const Block *block, uint8_t out_hash[32])
{
if(!block || !out_hash) return;
uint8_t buffer[HEADER_SIZE];
block_serialize_header(block, buffer);
double_sha256(buffer, HEADER_SIZE, out_hash);
}


void block_calculate_hash(Block *block)
{
    if(!block) return;
    block_calculate_hash_out(block, block->hash);
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
    memcpy(buffer + offset, &block->header, sizeof(BlockHeader));
    offset += sizeof(BlockHeader);

    memcpy(buffer + offset, &block->hash, 32);
    offset += 32;

    size_t txs_bytes = block->tx_count * sizeof(Transaction);
    
    if (block->tx_count >0 && block->transactions)
    {
    memcpy(buffer + offset, block->transactions, txs_bytes);
    offset += txs_bytes;
    }
    
    return offset;
}


Block * block_deserialize(const uint8_t *buffer, size_t length)
{

if(!buffer || length < sizeof(BlockHeader) + 32) return NULL;
  
    Block *block = (Block*)malloc(sizeof(Block));   
    if(!block) return NULL;
  
    memset(block,0, sizeof(Block));
    size_t offset=0;

    memcpy(&block->header, buffer + offset, sizeof(BlockHeader));
    offset += sizeof(BlockHeader);

    memcpy(block->hash, buffer + offset, 32);
    offset+=32;

    block->tx_count = block->header.tx_count;

    if(block->tx_count > 0)
    {
    size_t expected_tx_bytes = block->tx_count * sizeof(Transaction);
    
        if(length < offset + expected_tx_bytes)
        {
        free(block);
        return NULL;
        }
        
    block->transactions = (Transaction*)malloc(expected_tx_bytes);
    if(block->transactions){ mempcpy(block->transactions, buffer + offset, expected_tx_bytes);}
    }

    else
    {
    block->transactions = NULL;
    }


block->data = NULL;
return block;
}



void block_free(Block *block)
{
    if (block)
    {
        if (block->transactions)
        {
        free(block->transactions);
        }
        
        if (block->data)
        {
        free(block->data);
        }
    free(block);
    }
}


void merkle_tree_calculate_root(const Transaction *txs, uint32_t count, uint8_t root[32])
{
	if(!txs || count == 0)
	{
	memset(root, 0, 32);
	return;    
	}

	if(count == 1)
	{
	sha256((const uint8_t*)&txs[0], sizeof(Transaction),root);
	return;
	}

size_t level_count = count;
uint8_t (*current_level)[32] = malloc(level_count * 32);

	if(!current_level)
	{
	memset(root, 0, 32);
	return;
	}


	for(size_t i=0; i< count; i++)
	{
	sha256((const uint8_t*)&txs[i], sizeof(Transaction), current_level[i]);
	}

	while(level_count > 1)
	{
	size_t next_level_count = (level_count + 1) /2;
	uint8_t (*next_level)[32] = malloc(next_level_count * 32);

		if(!next_level)
		{
		free(current_level);
		memset(root,0,32);
		return;
		}
		
		for(size_t i = 0; i < level_count; i+=2)
		{
		uint8_t combined[64];
		memcpy(combined, current_level[i], 32);
				if(i + 1 <level_count) { memcpy(combined + 32, current_level[i + 1], 32);}
				else { memcpy(combined + 32, current_level[i], 32);}
				sha256(combined, 64, next_level[i / 2]);
		}
		
		free(current_level);
		current_level = next_level;
		level_count = next_level_count;
	}
	
	memcpy(root, current_level[0], 32);
	free(current_level); 
}



int check_proof_of_work(const uint8_t hash[32], uint32_t diff_bits)
{
	uint32_t full_bytes = diff_bits / 8;
	uint32_t remaining_bits = diff_bits % 8;
	
	for(uint32_t i=0; i < full_bytes; i++)
    {
	if(hash[i]!= 0x00) return 0;
	}
		
	if (remaining_bits > 0)
    {
	uint8_t mask = (uint8_t)(0xFF << (8 - remaining_bits));
	if((hash[full_bytes] & mask) != 0) return 0;
	}
	return 1;
}

int block_verify_pow(const Block * block, uint32_t diff_bits)
{
    if(!block) return 0;
    uint8_t recalculated_hash[32];

    block_calculate_hash_out(block, recalculated_hash);
    
    if(memcmp(block->hash, recalculated_hash, 32) !=0)
    {
	fprintf(stderr, "[!] Error POW: El hash guardado no coincide con el hash recalculado. \n.");
    return 0;
	}
	
    if(!check_proof_of_work(recalculated_hash, diff_bits))
    {
	fprintf(stderr, "[!] Error POW: El hash no cumple con la dificultad exigida. \n");
	return 0;
	}
return 1;
}




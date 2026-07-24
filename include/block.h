#ifdef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <stdef.h>
#include <time.h>

#define HEADER_SIZE 80

typedef struct{
uint32_t version;
uint8_t prev_hash[32];
uint8_t merkle_root[32];
uint32_t timestamp;
uint32_t bits;
uint32_t nonce;
    } BlockHeader;


typedef struct{
Blockheader header;
uint8_t hash[32];
char *data;
} Block;

Block* block_create(const uint8_t prev_hash[32], const char *data, uint32_t bits);
void block_serialize_header(const Block *block, uint8_t buffer[HEADER_SIZE]);
void block_calculate_hash(Block *block);
void block_free(Block *block);
#endif


#include <stdio.h>
#include <string.h>
#include "../lib/utils.h"
#include "../lib/block.h"
#include "../lib/blockchain.h"

void test_stage1_hashing()
{
    printf("------------- STAGE 1: Hashing SHA 256 Test ------------------\n");
    uint8_t out[32];
    char hex[65];
    sha256((const uint8_t*)"", 0, out);
    hex_encode(out, 32, hex);
    printf("SHA-256(\"\"): %s\n", hex);
}

int main()
{
    test_stage1_hashing();

    uint32_t diff_bits = 16;
    char hex_hash[65];

    printf("\n--- Loading / Blockchain Initailization (File: %s) ---\n", CHAIN_FILE);
    

    Blockchain *chain = blockchain_init(diff_bits, CHAIN_FILE);
    if (!chain) {
        fprintf(stderr, "Error to initialize blockchain\n");
        return 1;
    }

    
    printf("\n[Chain Status] Current Long: %zu Block(s)\n", chain->length);
    for (size_t i = 0; i < chain->length; i++) {
        hex_encode(chain->blocks[i]->hash, 32, hex_hash);
        printf("  - Block [%zu] | Hash: %s | Nonce: %u\n", 
               i, hex_hash, chain->blocks[i]->header.nonce);
    }

    
    Block *last_block = chain->blocks[chain->length - 1];

    
    printf("\nMining new block [%zu]...\n", chain->length);
    
    char tx_data[64];
    snprintf(tx_data, sizeof(tx_data), "Tx %zu: Alice -> Bob (%zu BTC)", chain->length, chain->length * 5);

    Block *new_block = block_create(last_block->hash, tx_data, diff_bits);
    mine_block(new_block, diff_bits);

    
    if (blockchain_add_block(chain, new_block)) {
        blockchain_save_block(new_block, CHAIN_FILE);
        hex_encode(new_block->hash, 32, hex_hash);
        printf("[+] Bloque [%zu] saved on disk | Hash: %s\n", chain->length - 1, hex_hash);
    }

    
    printf("\nValid chain?: %s\n",
        blockchain_is_valid(chain) ? "YES (OK)" : "NO (CORRUPT)");


    blockchain_free(chain);
    return 0;
}
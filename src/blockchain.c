#include "../lib/blockchain.h"
#include "../lib/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int blockchain_is_chain_valid(Block ** blocks, size_t length, uint32_t diff_bits)
{
    if (!blocks || length == 0) return 0;
   
    for (size_t i = 0; i < length; i++)
    {
    Block *current = blocks[i];
    if(!current) return 0;
    
    // validate pow in the block
    if(!block_verify_pow(current, diff_bits)) { fprintf(stderr, "[!] Fork Reject: Invalid POW in the block %zu\n", i); return 0; }
    
    // validate the hash continuity from the block 1
    if (i > 0)
    {
    Block *previous = blocks[i - 1];
        if(memcmp(current->header.prev_hash, previous->hash, 32) != 0)
        {
        fprintf(stderr, "[!] Fork reject: Discrepancy form prev_hash in the block %zu\n", i);
        return 0;
        }
    }
    }
    return 1;
}


int blockchain_resolve_fork(Blockchain *chain, Block **new_blocks, size_t new_length)
{
    if(!chain || !new_blocks) return 0; 

    if(new_length <= chain->length)
    {
    printf("[+] Remote chain is not longest (%zu <= %zu) keeping the current chain. \n", new_length, chain->length);
    return 0;
    }
    
printf("[*] Evaluating fork resolution. Local chain: %zu | Incoming chain: %zu...\n", chain->length, new_length);

    // validate the complette integrity from the new received chain

    if(!blockchain_is_chain_valid(new_blocks, new_length, chain->difficulty_bits)) 
    {
    fprintf(stderr,"[!] Fork rejected: The incomming chain contains invalid blocks. \n");
    return 0;
    }
    
    // free old blocks
    for (size_t i = 0; i < chain->length; i++ )
    {
    if(chain->blocks[i]){ block_free(chain->blocks[i]); }
    }
    
    free(chain->blocks);
    
    //assign the new chain
    chain->blocks = new_blocks;
    chain->length = new_length;
    
    //persist in the disk
    FILE *fp = fopen(CHAIN_FILE, "wb");
    
    if(fp)
    {
        for(size_t i=0; i < chain->length; i++)
        {
        uint8_t buffer[1024];
        size_t bytes = block_serialize(chain->blocks[i], buffer);
        fwrite(buffer, 1, bytes, fp);
        }
    fclose(fp);
    }

printf("[+] Fork resolved: local chain updated sucessfully in %zu blocks \n", new_length);
return 1;
}


int meets_difficulty(const uint8_t hash[32], uint32_t difficulty_bits)
{
    uint32_t full_bytes = difficulty_bits / 8;
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
    block->header.bits = difficulty_bits;
    block->header.nonce = 0;
    
    do {
        block->header.nonce++;
        block_calculate_hash(block);
        }
        while (!meets_difficulty(block->hash, difficulty_bits));
}

Blockchain* blockchain_create(uint32_t difficulty_bits)
{
    Blockchain *chain = (Blockchain*)malloc(sizeof(Blockchain));
    if (!chain) return NULL;

    // Build the origin transaction (Genesis)
    Transaction genesis_tx;
    transaction_create(&genesis_tx, "SYSTEM", "GENESIS_REWARD_ADDRESS", 50.0);
    strncpy(genesis_tx.data, "Genesis Block: Genesis 2026", TX_DATA_LEN - 1);

    // Build the genesis block
    Block *genesis = block_create(NULL, &genesis_tx, 1, difficulty_bits);
    if (!genesis) {
        free(chain);
        return NULL;
    }

    // Mine the genesis block
    mine_block(genesis, difficulty_bits);

    // Asign the chain array
        
        chain->blocks = (Block**)malloc(sizeof(Block*));
        chain->blocks[0] = genesis;
        chain->length = 1;
        chain->difficulty_bits = difficulty_bits;

    return chain;
}


int blockchain_add_block(Blockchain *chain, Block *new_block)
{
    Block *prev_block = chain->blocks[chain->length - 1];
    if(memcmp(new_block->header.prev_hash, prev_block->hash, 32) != 0)
    {
        printf("[Error] previous hash mismatch");
        return 0;
        }

        if(!meets_difficulty(new_block->hash, chain->difficulty_bits))
        {
            printf("[Error] The block not accept the exiged quality");
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
    uint8_t recalculated[32];
    
    for(size_t i = 0; i < chain->length; i++)
    { 
        Block *current = chain->blocks[i];
        
        // recalculate hash
        uint8_t buf[HEADER_SIZE];
        block_serialize_header(current, buf);
        double_sha256(buf, HEADER_SIZE, recalculated);
        if(memcmp(recalculated, current->hash, 32) !=0 ) return 0;
        
        // validate hash from the early chain

        if(i > 0){
            Block *prev = chain->blocks[i - 1];
            if(memcmp(current->header.prev_hash, prev->hash, 32) !=0) return 0;
            }

            //verify difficulty

            if(!meets_difficulty(current->hash, chain->difficulty_bits)) return 0;
    }
    return 1;
}

int blockchain_save_block(const Block *block, const char *filepath)
{
    FILE * file = fopen(filepath, "ab");
    if(!file) return -1;
    uint8_t buf[HEADER_SIZE];
    block_serialize_header(block, buf);
    size_t written = fwrite(buf, HEADER_SIZE, 1, file);
    fclose(file);
    return (written == 1) ? 0 : -1;
}


Blockchain* blockchain_init(int32_t difficulty_bits, const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if(!file){
        printf("[+] previous chain not found, generate blockchain...\n");
        Blockchain *chain = blockchain_create(difficulty_bits);

        if(chain && chain->length > 0)
        {
            blockchain_save_block(chain->blocks[0], filepath);
            printf("[+] block genesis saved sucessfully in '%s'.\n",filepath);
            }
            return chain;
            }

            printf("[+] file '%s' saved sucessfully. loading blockchain",filepath);
            Blockchain *chain = (Blockchain*)malloc(sizeof(Blockchain));
            
            chain->capacity=10;
            chain->length=0;
            chain->difficulty_bits = difficulty_bits;
            chain->blocks = (Block**)malloc(sizeof(Block*) * chain->capacity);
            uint8_t buf[HEADER_SIZE];

            while(fread(buf,HEADER_SIZE,1,file)==1)
            {
                if(chain->length >= chain->capacity)
                {
                    chain->capacity *=2;
                    chain->blocks = (Block**)realloc(chain->blocks,sizeof(Block*) * chain->capacity);
                    }

                    Block *b = (Block*) calloc(1, sizeof(Block));
                    double_sha256(buf, HEADER_SIZE, b->hash);
                    chain->blocks[chain->length++] = b;
                    }

                    fclose(file);
                    printf("[+] Loading complete. total blocks in the chain %zu\n", chain->length);
                    return chain;
                    }


void blockchain_free(Blockchain *chain)
{
    if(chain)
    {
        for(size_t i=0;i < chain->length; i++)
        {
            block_free(chain->blocks[i]);
            }
            free(chain->blocks);
            free(chain);
    }
}

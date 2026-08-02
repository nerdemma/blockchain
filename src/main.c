#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../lib/blockchain.h"
#include "../lib/net.h"
#include "../lib/mempool.h"

#include "../lib/utils.h"
#include "../lib/block.h"
#include "../lib/discovery.h"

void print_menu()
{
printf("\n=========BLOCKCHAIN NODE =========\n");
printf("1. view the blockchain status\n");
printf("2. Mining the block with pending transactions\n");
printf("3. Create testing transaction\n");
printf("4. Connect to peer p2p\n");
printf("5. Exit\n");
printf("Select Option:");
}


void test_stage1_hashing()
{
    printf("------------- STAGE 1: Hashing SHA 256 Test ------------------\n");
    uint8_t out[32];
    char hex[65];
    sha256((const uint8_t*)"", 0, out);
    hex_encode(out, 32, hex);
    printf("SHA-256(\"\"): %s\n", hex);
}

int main(int argc, char *argv[])
{
uint32_t diff_bits = 16;
uint16_t tcp_port = DEFAULT_PORT;

if(argc > 1){ tcp_port= (uint16_t)atoi(argv[1]);}
Blockchain *chain = blockchain_init(diff_bits, CHAIN_FILE);
Mempool mp;
mempool_init(&mp);

// initialize the p2p server at second layer or config port
int server_fd = start_server(tcp_port);
int option = 0;

discovery_start(tcp_port);

while(option !=5)
{

    print_menu();
    if(scanf("%d",&option) !=1 ) break;
    
    switch(option)
    {
    case 1:
    printf("[Local Chain] Total Blocks: %zu | valid %s\n",
    chain->length, blockchain_is_valid(chain)? "YES" : "NO");
        for(size_t i = 0; i < chain->length; i++)
        {
        printf("- Bloque [%zu] Hash loaded sucessfully\n", i);   
        }
    break;     
    
    case 2:
    {
    Block *last = chain->blocks[chain->length -1];
    printf("\nMining block [%zu]...\n", chain->length);
    Block *new_block = block_create(last->hash,"Mempool block Tx",diff_bits);
    mine_block(new_block,diff_bits);    
    
    if(blockchain_add_block(chain,new_block))
    {
    blockchain_save_block(new_block, CHAIN_FILE);
    mempool_clear(&mp); 
    printf("[+] Block mined and persisted sucessfully\n");   
    }
    break;
    }
   
    case 3:
    {
    Transaction tx;
    mempool_add_tx(&mp, &tx);
    break;   
    }

    case 4:
    {
    char ip[64];
    int p;
    printf("PEER IP ADDRESS: ");
    scanf("%s", ip);
    printf("PORT: ");    
    scanf("%d", &p);
    
    int peer_fd = connect_to_peer(ip, p);    
    if(peer_fd >= 0)
    {
        MsgVersion v = {1, 1700000000, chain->length};
        send_message(peer_fd, MSG_VERSION, &v, sizeof(MsgVersion));
    }    
    break;
    }

    case 5:
    printf("Closing..\n");
    break;

    default:
    printf("Option not valid\n");
    break;
}
}

if (server_fd >= 0) close(server_fd);
    blockchain_free(chain);
    return 0;
}

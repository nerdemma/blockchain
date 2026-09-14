#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../lib/blockchain.h"
#include "../lib/net.h"
#include "../lib/mempool.h"
#include "../lib/discovery.h"
#include "../lib/peer_pool.h"
#include "../lib/p2p_sync.h"


int main(int argc, char *argv[])
{

uint16_t tcp_port = DEFAULT_PORT;

if(argc > 1)
{
tcp_port = (uint16_t)atoi(argv[1]);
}

peer_pool_init(&g_peer_pool);
Blockchain *chain = blockchain_init(CHAIN_FILE);

if(!chain)
{
fprintf(stderr,"[Critical Error] Blockchain not initializng.\n");
return 1;
}

Mempool mp;
mempool_init(&mp);
mempool_load(&mp, MEMPOOL_FILE);
printf("[Initialing] Demon P2P. \n[Mempool] Loaded with %zu transactions.\n", mp.count);
int server_fd = start_server(tcp_port);
discovery_start(tcp_port);

if (p2p_sync_start(server_fd, chain, &mp, &g_peer_pool) == 0)
{
printf("[Demon P2P] Running in port:%d. Press Ctrl+C to exit \n", tcp_port);
}


while(1)
{
sleep(1);
}


p2p_sync_stop();
if(server_fd >=0) close(server_fd);
mempool_clear(&mp);
blockchain_free(chain);
return 0;
}

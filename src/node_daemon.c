#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../lib/blockchain.h"
#include "../lib/net.h"
#include "../lib/mempool.h"
#include "../lib/discovery.h"
#include "../lib/peer_pool.h"
#include "../lib/p2p_sync.h"

PeerPool g_peer_pool;
int main(int argc, char *argv[])
{
uint32_t diff_bits = 16;
uint16_t tcp_port = DEFAULT_PORT;

if(argc > 1)
{
tcp_port = (uint16_t)atoi(argv[1]);
}

peer_pool_init(&g_peer_pool);
Blockchain *chain = blockchain_init(diff_bits, CHAIN_FILE);

if(!chain)
{
fprintf(stderr,"[!] Error critico: No se pudo inicializar la blockchain. \n");
return 1;
}

Mempool mp;
mempool_init(&mp);

int server_fd = start_server(tcp_port);
discovery_start(tcp_port);

if (p2p_sync_start(server_fd, chain, &mp, &g_peer_pool) == 0)
{
printf("[+] Demon P2P corriendo en el puerto %d. Preciona Ctrl+C para salir \n", tcp_port);
}

// bucle del demon en segundo plano
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

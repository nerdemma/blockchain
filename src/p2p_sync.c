#include "../lib/p2p_sync.h"
#include "../lib/net.h"          
#include "../lib/block.h"
#include "../lib/transaction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>

P2PServerContex g_p2p_ctx = {0};

static void handle_peer_message(int peer_fd, uint8_t type,const uint8_t *payload, uint32_t length)
{
(void)peer_fd;    
switch(type)
{
    case MSG_VERSION:
    {
    if(length >= sizeof(MsgVersion))
    {
    MsgVersion *v = (MsgVersion*)payload;
    printf("\n[P2P] MsgVersion recibido: Versión=%u", v->version);
    }
    break;
    }

    case MSG_TX:
    {
        if(length == sizeof(Transaction))
        {
        Transaction tx;
        memcpy(&tx, payload, sizeof(Transaction));
        if(transaction_is_valid(&tx))
        {
            printf("[P2P] New recieved transaction and added in mempool\n");
        }

        }
    
    break;    
    }

    case MSG_BLOCK:
    {
        Block *incoming_block = block_deserialize(payload, length);
        if(incoming_block)
        {
        if(blockchain_add_block(g_p2p_ctx.chain,incoming_block))
        {
        blockchain_save_block(incoming_block, CHAIN_FILE);
        printf("\n[P2P] New block received in the network and validaded in the local blockchain.\n");         
        }    
        else
        {
        block_free(incoming_block);    
        }
        }
    break;
    }
    default:
    break;

}
}


static void* p2p_listener_thread(void *arg)
{
 (void)arg;
 fd_set read_fds;
 struct timeval timeout;

    while(g_p2p_ctx.running)
    {
        FD_ZERO(&read_fds);
        int max_fd = g_p2p_ctx.server_fd;

        if(g_p2p_ctx.server_fd >= 0)
        {
        FD_SET(g_p2p_ctx.server_fd,&read_fds);
        }

        // agregar los peers activos del pool al conjunto select
        pthread_mutex_lock(&g_p2p_ctx.peer_pool->lock);

        for(size_t i = 0; i < g_p2p_ctx.peer_pool->count; i++)
        {
        int pfd = g_p2p_ctx.peer_pool->peers[i].socket_fd;
        if(pfd >= 0)
        {
        FD_SET(pfd, &read_fds);
        if(pfd > max_fd) max_fd = pfd;   
        }

        pthread_mutex_unlock(&g_p2p_ctx.peer_pool->lock);  
        timeout.tv_sec=1;
        timeout.tv_usec=0;
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if(activity < 0 || !g_p2p_ctx.running) continue;

        if(g_p2p_ctx.server_fd >= 0 && FD_ISSET(g_p2p_ctx.server_fd, &read_fds))
        {   
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int new_socket = accept(g_p2p_ctx.server_fd, (struct sockaddr*)&client_addr, &addr_len);

        if(new_socket >=0)
        {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        uint16_t client_port = ntohs(client_addr.sin_port);
        peer_pool_add(g_p2p_ctx.peer_pool, new_socket, client_ip, client_port); 
        }
        }

        // leer los datos de los peer conectados
        pthread_mutex_lock(&g_p2p_ctx.peer_pool->lock);

        for (size_t i = 0; i < g_p2p_ctx.peer_pool->count; i++)
        {
        int pfd = g_p2p_ctx.peer_pool->peers[i].socket_fd;
                if(pfd >=0 && FD_ISSET(pfd, &read_fds))
                {               
                uint8_t msg_type;
                uint32_t payload_len;
                uint8_t *payload = NULL;

                    if(receive_message(pfd, &msg_type, &payload, &payload_len) == 0)
                    {
                    handle_peer_message(pfd, msg_type, payload, payload_len);
                    if(payload) free(payload);
                    }
                    else
                    {
                    close(pfd);
                    peer_pool_remove(g_p2p_ctx.peer_pool, pfd);
                    i--;
                    }   
                }
        }
    }        
pthread_mutex_unlock(&g_p2p_ctx.peer_pool->lock);

}
return NULL;
}


int p2p_sync_start(int server_fd, Blockchain *chain, Mempool *mempool, PeerPool *peer_pool)
{
g_p2p_ctx.server_fd = server_fd;
g_p2p_ctx.chain = chain;
g_p2p_ctx.mempool = mempool;
g_p2p_ctx.peer_pool = peer_pool;
g_p2p_ctx.running = true;

if (pthread_create(&g_p2p_ctx.thread, NULL, p2p_listener_thread, NULL) != 0) 
{
perror("[!] Error al crear el hilo P2P \n"); 
return -1;   
}
return 0;
}

void p2p_sync_stop(void)
{
    if(g_p2p_ctx.running){

        g_p2p_ctx.running=false;
        pthread_join(g_p2p_ctx.thread, NULL);
    }
}

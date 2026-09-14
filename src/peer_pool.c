
#include "../lib/peer_pool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

PeerPool g_peer_pool;


void peer_pool_init(PeerPool *pool)
{
pool->count = 0;
pthread_mutex_init(&pool->lock, NULL);
    for(int i=0; i < MAX_PEERS; i++)
    {
        pool->peers[i].is_active = 0;
    }
}

int peer_pool_add(PeerPool *pool, int socket_fd, const char *ip, uint16_t port)
{
pthread_mutex_lock(&pool->lock);
    for(int i=0; i < MAX_PEERS; i++)
    {
 
    if(!pool->peers[i].is_active)
    {
    pool->peers[i].socket_fd = socket_fd;
    strncpy(pool->peers[i].ip, ip, sizeof(pool->peers[i].ip) -1);

    pool->peers[i].port = port;
    pool->peers[i].is_active=1;
    
    pool->count++;
    pthread_mutex_unlock(&pool->lock);
    printf("[P2P Pool] Peer %s:%u added (FD: %d)\n", ip, port, socket_fd);
    return 0;
    }   
    }

    pthread_mutex_unlock(&pool->lock);
printf("[P2P Pool] Pair pool fulled\n");
return -1;
}

void peer_pool_remove(PeerPool *pool, int socket_fd)
{
pthread_mutex_lock(&pool->lock);
for(int i = 0; i < MAX_PEERS; i++)
{
    if(pool->peers[i].is_active && pool->peers[i].socket_fd == socket_fd)
    {
        pool->peers->is_active=0;
        pool->count--;
        printf("[P2P Pool] Peer FD %d removed.\n", socket_fd);
        break;
    }
  }
pthread_mutex_unlock(&pool->lock);
}

void peer_pool_broadcast(PeerPool *pool, MessageType type, const void *payload, uint32_t payload_size)
{
pthread_mutex_lock(&pool->lock);

for(int i=0; i < MAX_PEERS; i++)
{
    if(pool->peers[i].is_active)
    {
    if(send_message(pool->peers[i].socket_fd, type, payload, payload_size) < 0)    
    {
    close(pool->peers[i].socket_fd); 
    pool->peers[i].is_active = 0; 
    pool->count --;  
    }
    }

}
pthread_mutex_unlock(&pool->lock);
}

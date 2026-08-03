#ifndef PEER_POOL_H
#define PEER_POOL_H

#include <stdint.h>
#include <pthread.h>
#include "../lib/net.h"

#define MAX_PEERS 10

typedef struct{
int socket_fd;
char ip[64];
uint16_t port;
int is_active;
} Peer;

typedef struct{
Peer peers[MAX_PEERS];
size_t count;
pthread_mutex_t lock;
} PeerPool;

extern PeerPool g_peer_pool;


void peer_pool_init(PeerPool *pool);
int peer_pool_add(PeerPool *pool, int socket_fd, const char *ip, uint16_t port);
void peer_pool_remove(PeerPool *pool, int socket_fd);
void peer_pool_broadcast(PeerPool *pool, MessageType type, const void *payload, uint32_t payload_size);

#endif
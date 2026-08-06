#ifndef P2P_SYNC_H
#define P2P_SYNC_H
#include <pthread.h>
#include <stdbool.h>
#include "blockchain.h"
#include "mempool.h"
#include "peer_pool.h"

typedef struct {
int server_fd;
Blockchain *chain;
Mempool *mempool;
PeerPool *peer_pool;
bool running;
pthread_t thread;
} P2PServerContex;

extern P2PServerContex g_p2p_ctx;

int p2p_sync_start(int serverfd, Blockchain *chain,  Mempool *mempool, PeerPool *peer_pool);
void p2p_sync_stop(void);


#endif //P2P_SYNC_H
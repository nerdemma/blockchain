#include "../lib/discovery.h"
#include "../lib/net.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static uint16_t  g_local_tcp_port = DEFAULT_PORT;

static void* broadcast_sender_thread(void *arg){
(void)arg;
int sock = socket(AF_INET, SOCK_DGRAM, 0);

if(sock < 0)
{
perror("[Discovery] Error to create transmiter socket UDP");
return NULL;
}

// enable the option SO_BROADCAST in the socket
int broadcast_enable=1;
if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0)
{
perror("[Discovery] Error config SO_BROADCAST");
close(sock);
return NULL;
}
struct sockaddr_in broadcast_addr;
memset(&broadcast_addr, 0, sizeof(broadcast_addr));

broadcast_addr.sin_family = AF_INET;
broadcast_addr.sin_port = htons(DISCOVERY_PORT);
broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

DiscoveryBeacon beacon;
beacon.magic = MAGIC_DISCOVERY;
beacon.tcp_port = htons(g_local_tcp_port);
printf("[Discovery] Emisor UDP iniciado Transmitiendo cada %d segundos...\n", BROADCAST_INTERVAL_SEC);

while(1)
{
ssize_t sent = sendto(sock,&beacon, sizeof(beacon),0,
(struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));

if(sent<0)
{
perror("[Discovery] Error al enviar el paquete broadcast");    
}
sleep(BROADCAST_INTERVAL_SEC);
}
close(sock);
return NULL;
}

// receptor thead: listen messages from others nodes of lan
static void* broadcast_listener_thread(void *arg)
{
(void)arg;
int sock = socket(AF_INET, SOCK_DGRAM, 0);

if(sock < 0) 
{
printf("[Error] Error to create receptor UDP");
return NULL;
}

int reuse = 1;
setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
struct sockaddr_in listen_addr;
memset(&listen_addr, 0, sizeof(listen_addr));
listen_addr.sin_family = AF_INET;
listen_addr.sin_port = htons(DISCOVERY_PORT);
listen_addr.sin_addr.s_addr = INADDR_ANY;

if(bind(sock,(struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0)
{
perror("[DISCOVERY] Error to binding receptor socket UDP");
}

printf("[DISCOVERY] Receptor UDP structured in the port %d...\n",DISCOVERY_PORT);
DiscoveryBeacon beacon;
struct sockaddr_in sender_addr;
socklen_t addr_len = sizeof(sender_addr);

while(1)
{
ssize_t bytes = recvfrom(sock, &beacon, sizeof(beacon), 0,
(struct sockaddr*)&sender_addr, &addr_len);

if(bytes == sizeof(DiscoveryBeacon))
{
    if(beacon.magic == MAGIC_DISCOVERY)
    {
    char sender_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sender_addr.sin_addr), sender_ip, INET_ADDRSTRLEN);   
    uint16_t peer_tcp_port = ntohs(beacon.tcp_port);
    
        if(peer_tcp_port != g_local_tcp_port || strcmp(sender_ip, "127.0.0.1") != 0)
        {
        printf("\n[DISCOVERY] Node detected in LAN -> %s:%u (TCP P2P)\n", sender_ip, peer_tcp_port);   
        // function to autoconnect
        }
    }
}
}
close(sock);
return NULL;
}


int discovery_start(uint16_t local_tcp_port) {
    g_local_tcp_port = local_tcp_port;

    pthread_t sender_tid, listener_tid;

    if (pthread_create(&sender_tid, NULL, broadcast_sender_thread, NULL) != 0) {
        return -1;
    }
    pthread_detach(sender_tid);

    if (pthread_create(&listener_tid, NULL, broadcast_listener_thread, NULL) != 0) {
        return -1;
    }
    pthread_detach(listener_tid);

    return 0;
}



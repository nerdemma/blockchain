#include "../lib/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

int start_server(int port)
{
int server_fd;
struct sockaddr_in address;
int opt=1;

if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
{
    perror("error to create socket");
    return -1;
}

if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
{
perror("Error en setsockopt");
close(server_fd);
return -1;
}

address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(port);

if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) <0 )
{
    perror("Bind Error");
    close(server_fd);
    return -1;
}

if(listen(server_fd, 5) < 0)
{
perror("Listen Error");
close(server_fd);
return -1;
}

printf("[P2P] Server listening in the port %d...\n", port);
return server_fd;
}

int connect_to_peer(const char *ip, int port)
{
int fd = socket(AF_INET, SOCK_STREAM, 0);
if(fd < 0) return -1;

struct timeval tv;
tv.tv_sec=3;
tv.tv_usec=0;
setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

struct sockaddr_in addr;
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(port);
inet_pton(AF_INET, ip, &addr.sin_addr);

    if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
    close(fd);
    return -1;
    }
return fd;    
}

int send_message(int socket_fd, MessageType type, const void *payload, uint32_t payload_size)
{
Netheader header;
header.magic = MAGIC_BYTES;
header.type = type;
header.length = payload_size;

// send header
    if(send(socket_fd, &header, sizeof(Netheader),0) < 0)
    {
    return -1;
    }

// send payload if exists
if(payload_size > 0 && payload != NULL)
{
    if(send(socket_fd, payload, payload_size, 0) < 0)
    {
    return -1;    
    }
}
return 0;
}

int receive_message(int socket_fd, uint8_t *msg_type, uint8_t **payload, uint32_t *payload_len)
{
if (socket_fd < 0 || !msg_type || !payload || !payload_len) return 1;
if (recv(socket_fd, msg_type, 1, 0) <= 0) return -1;
if (recv(socket_fd,payload_len, sizeof(uint32_t),0) <=0 ) return -1;

if(*payload_len > 0)
{
*payload = (uint8_t*)malloc(*payload_len);
if(!payload) return -1;
ssize_t bytes_read = recv(socket_fd, *payload, *payload_len, MSG_WAITALL);

if(bytes_read <=0)
{
free(*payload);
*payload = NULL;
return -1;
}
else
{
*payload = NULL;
}
}
return 0;
}


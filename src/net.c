#include "../lib/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
int sock=0;
struct sockaddr_in serv_addr;

if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
printf("\n Error to create client socket \n");
return -1;
}

serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET,ip, &serv_addr.sin_addr)<=0)
    {
    printf("\n IP Address Invalid or Not Supported\n");
    return -1;
    }

    if(connect(sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr) <= 0))
    {
        printf("conection failed by peer");
        return -1;
    }

    printf("[P2P] Connected sucessfully with the peer: %s:%d\n", ip, port);
    return sock;
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
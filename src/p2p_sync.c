#include "../lib/net.h"
#include "../lib/blockchain.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

void handle_peer_connection(int peer_fd, Blockchain *chain)
{
Netheader header;
//read header
    if(recv(peer_fd, &header, sizeof(Netheader),0) <= 0)
    {
    close(peer_fd); 
    return;  
    }

if(header.magic != MAGIC_BYTES)
{
    printf("[Error] Invalid package recived by magic bytes \n");
    close(peer_fd);
    return;
}

switch (header.type)
{
case MSG_VERSION:
{
MsgVersion v;
recv(peer_fd, &v, sizeof(MsgVersion),0);

printf("[P2P] Peer version recived. Chain hight: %zu (Our: %zu)\n"
,v.chain_length, chain->length);

// respond by VERACK
send_message(peer_fd, MSG_VERRACK, NULL, 0);

// if the peeer have a long chain, give their blocks
if(v.chain_length > chain->length)
{
printf("[CONSENSUS] The peer chain is too long. petition blocks \n");
send_message(peer_fd, MSG_GETBLOCKS, NULL, 0);
}
break;
}
case MSG_GETBLOCKS:
{
printf("[P2P] peer giving our chain, send blocks...\n");
// transmit each block serialized by peer
    for (size_t i = 0; i < chain->length; i++)
    {
        uint8_t buf[HEADER_SIZE];
        block_serialize_header(chain->blocks[i], buf);
        send_message(peer_fd, MSG_BLOCK, buf, HEADER_SIZE);
    }
break;
}
default:
printf("[P2P] Type of message not driving: %u\n", header.type);
break;
}


}

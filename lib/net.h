#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_PORT 8333
#define MAGIC_BYTES 0xD9B4BEF9

// tipos de mensaje
typedef enum {
    MSG_VERSION = 1,
    MSG_TX      = 2,
    MSG_BLOCK   = 3,
    MSG_TRANSACTION = 1,
    MSG_PRE_PREPARE,
    MSG_PREPARE, 
    MSG_COMMIT,
    MSG_ROUND_CHANGE,
} MessageType;

typedef struct __attribute__((__packed__)) {
 uint32_t magic;
 uint32_t type;
 uint32_t length;
} Netheader;

typedef struct __attribute__((__packed__))
{
uint32_t version;
uint32_t timestamp;
size_t chain_length;
} MsgVersion;

int send_message(int socket_fd, MessageType type, const void *payload, uint32_t payload_size);
int receive_message(int socket_fd, uint8_t *msg_type, uint8_t **payload, uint32_t *payload_len);

int start_server(int port);
int connect_to_peer(const char *ip, int port);
#endif // NET_H


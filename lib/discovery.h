#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <stdint.h>
#define DISCOVERY_PORT 8334
#define BROADCAST_INTERVAL_SEC 5
#define MAGIC_DISCOVERY 0x4E455244 // "NERD" en ASCII/Hex


// payload UDP package discovery
typedef struct __attribute__((__packed__)) {
    uint32_t magic;
    uint16_t tcp_port;
}DiscoveryBeacon;

int discovery_start(uint16_t local_tcp_port);

#endif //DISCOVERY_H

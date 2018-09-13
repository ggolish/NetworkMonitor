#ifndef PACKETS_H_
#define PACKETS_H_

#include <stdint.h>

// Defines ethernet payload types
#define ETH_TYPE_IP4 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP6 0x86DD

// Ethernet packet header
typedef struct __attribute__((packed)) {
    uint8_t  eth_mac_dest[6]; // The destination MAC address
    uint8_t  eth_mac_src[6];  // The source MAC address
    uint16_t eth_type;        // The type of the payload
} PACKET_ETH_HDR;

#define ARP_OPER_REQUEST 0x0001
#define ARP_OPER_REPLY   0x0002

// ARP packet header
typedef struct __attribute__((packed)) {
    uint16_t arp_htype; // Specifies the link layer protocol
    uint16_t arp_ptype; // Specifies the protocol the ARP packet is intended for
    uint8_t  arp_hlen;  // Length of the hardware address
    uint8_t  arp_plen;  // Length of the protocol address
    uint16_t arp_oper;  // The operation the sender is performing (request or reply)
} PACKET_ARP_HDR;

#endif

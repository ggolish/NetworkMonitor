#ifndef PACKETS_H_
#define PACKETS_H_

#include <stdint.h>

// Defines ethernet payload types
#define ETH_TYPE_IP4 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP6 0x86DD
#define ETH_TYPE_NETRANS 0x0929

// Ethernet packet header
typedef struct __attribute__((packed)) {
    uint8_t  eth_mac_dest[6]; // The destination MAC address
    uint8_t  eth_mac_src[6];  // The source MAC address
    uint16_t eth_type;        // The type of the payload
} PACKET_ETH_HDR;

// Defines the operation types of an ARP packet
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

// Defines IP protocol numbers
#define IP_PROTOCOL_ICMP    0x01
#define IP_PROTOCOL_IGMP    0x02
#define IP_PROTOCOL_TCP     0x06
#define IP_PROTOCOL_UDP     0x11
#define IP_PROTOCOL_IP6ICMP 0x3A

// IPv4 packet header
typedef struct __attribute__((packed)) {
    uint8_t  ip4_vers_ihl;
    uint8_t  ip4_dscp_ecn;
    uint16_t ip4_tlen;
    uint16_t ip4_iden;
    uint16_t ip4_flags_frag;
    uint8_t  ip4_ttl;
    uint8_t  ip4_protocol;
    uint16_t ip4_checksum;
    uint8_t  ip4_src[4];
    uint8_t  ip4_dest[4];
} PACKET_IP4_HDR;

// IPv6 packet header
typedef struct __attribute__((packed)) {
    uint8_t ip6_junk[6];
    uint8_t ip6_protocol;
    uint8_t ip6_hop;
    uint16_t ip6_src[8];
    uint16_t ip6_dest[8];
} PACKET_IP6_HDR;

// Defines the types of netrans packets
#define NETRANS_TYPE_SEND     0x01
#define NETRANS_TYPE_RECEIVE  0x02
#define NETRANS_TYPE_ACK      0x03
#define NETRANS_TYPE_CHUNK    0x04

// Custom netrans header
typedef struct __attribute__((packed)) {
    uint8_t netrans_src;
    uint8_t netrans_dest;
    uint8_t netrans_type;
} PACKET_NETRANS_HDR;

#endif

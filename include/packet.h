#ifndef PACKETS_H_
#define PACKETS_H_

// Defines ethernet payload types
#define ETH_TYPE_IP4 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP6 0x86DD

// Ethernet packet header
typedef struct __attribute__((packed)) {
    unsigned char eth_mac_dest[6];  // The destination MAC address
    unsigned char eth_mac_src[6];   // The source MAC address
    unsigned short eth_type;        // The type of the payload
} PACKET_ETH_HDR;

// ARP packet header
typedef struct __attribute__((packed)) {
    unsigned short arp_htype;       // Specifies the link layer protocol
    unsigned short arp_ptype;       // Specifies the protocol the ARP packet is intended for
    unsigned char arp_hlen;         // Length of the hardware address
    unsigned char arp_plen;         // Length of the protocol address
    unsigned short arp_oper;        // The operation the sender is performing (request or reply)
} PACKET_ARP_HDR;

#endif

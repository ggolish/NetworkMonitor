#include "netmon.h"
#include "errors.h"
#include "packet.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 

// Needed to check for device index
#include <sys/ioctl.h>
#include <net/if.h>

static void process_packet(char *packet_bytes, int len);
static void print_mac_address(unsigned char *address);

// Opens a raw socket and returns its file descriptor
int netmon_init(char *device_name)
{
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_ll sockaddr;

    // Open a raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    
    if(sockfd == -1) {
        sprintf(error_msg, "Unable to open raw socket (root privelidges required)");
        return -1;
    }

    // Ensure device_name is really a network device name and determine device index
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, device_name);
    if(ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        sprintf(error_msg, "Improper device name");
        return -1;
    }

    // Bind address to socket
    memset(&sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr.sll_family = AF_PACKET;
    sockaddr.sll_protocol = htons(ETH_P_ALL);
    sockaddr.sll_ifindex = ifr.ifr_ifindex;
    if(bind(sockfd, (struct sockaddr *)(&sockaddr), sizeof(struct sockaddr_ll)) == -1) {
        sprintf(error_msg, "Unable to bind address to socket");
        return -1;
    }

    return sockfd;
}

int netmon_mainloop(int sockfd)
{
    struct sockaddr_ll from;
    unsigned int len, addrlen;
    char buffer[4096];

    for(;;) {
        len = recvfrom(sockfd, buffer, 4096, 0, (struct sockaddr *)(&from), &addrlen);
        process_packet(buffer, len);
    }

    return 1;
}

static void process_packet(char *packet_bytes, int len)
{
    PACKET_ETH_HDR eth_hdr;
    PACKET_ARP_HDR arp_hdr;

    memcpy(&eth_hdr, packet_bytes, sizeof(PACKET_ETH_HDR));
    print_mac_address(eth_hdr.eth_mac_dest);
    print_mac_address(eth_hdr.eth_mac_src);

    switch(ntohs(eth_hdr.eth_type)) {
        case ETH_TYPE_IP4:
            printf("ipv4\n");
            break;
        case ETH_TYPE_IP6:
            printf("ipv6\n");
            break;
        case ETH_TYPE_ARP:
            printf("arp");
            memcpy(&arp_hdr, packet_bytes + sizeof(PACKET_ETH_HDR), sizeof(PACKET_ARP_HDR));
            switch(ntohs(arp_hdr.arp_oper)) {
                case ARP_OPER_REQUEST:
                    printf(" request\n");
                    break;
                case ARP_OPER_REPLY:
                    printf(" reply\n");
                    break;
                default:
                    printf(" unknown\n");
                    break;
            }
            break;
        default:
            printf("unkown\n");
            break;
    }

    printf("\n");
}

static void print_mac_address(unsigned char *address)
{
    for(int i = 0; i < 6; ++i) {
        printf("%02x", address[i]);
        if(i < 5) printf(":");
    }
    printf("\n");
}

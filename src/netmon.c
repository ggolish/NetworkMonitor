#include "netmon.h"
#include "errors.h"
#include "ui.h"
#include "packet.h"
#include "rate.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <time.h>

// Needed to check for device index
#include <sys/ioctl.h>
#include <net/if.h>

#define DEFAULT_NET_DEVICE "eth0"

// The length in seconds of each time block
#define TIME_BLOCK_LENGTH 1
#define TIME_BLOCK_AMOUNT 5

#define MACLENGTH 17 // The length of a mac address (with colons)
#define IP4LENGTH 15 // The length of an IPv4 address (with periods)
#define IP6LENGTH 39 // The length of an IPv4 address (with periods)

// The base amount for dynamic arrays
#define CHUNK 8

typedef struct __attribute__((packed)) {
    RATE_QUEUE *rq;    // A circular queue for maintaining the rate
    TIME_BLOCK *tb;    // The current block in the rate queue
    int arp_total;     // ARP packet total
    int ip4_total;     // IPv4 packet total
    int ip6_total;     // IPv6 packet total
    int reply_total;   // ARP reply packet total
    int request_total; // ARP request packet total
    int igmp_total;    // IGMP packet total
    int icmp_total;    // ICMP packet total
    int tcp_total;     // TCP packet total
    int udp_total;     // UDP packet total
    int total_bytes;   // The total number of bytes seen
    time_t total_time; // The total length of time for rate
    char **ip_addrs;   // The list of all IP addresses seen
    int ip_len;
    int ip_capacity;
    char **mac_addrs;  // The list of all MAC addresses seen
    int mac_len;
    int mac_capacity;
} NETMON;

static NETMON netmon;

static int skip_packet(char *packet_bytes, uint16_t mask);
static void process_packet(char *packet_bytes, int len, uint16_t mask);
static void process_ip4_packet(char *packet_bytes, char *mac_dest, char *mac_src);
static void process_ip6_packet(char *packet_bytes, char *mac_dest, char *mac_src);
static void process_arp_packet(char *packet_bytes, char *mac_dest, char *mac_src);

static void ip4_to_string(unsigned char *ip, char *buffer);
static void ip6_to_string(unsigned short *ip, char *buffer);
static void mac_to_string(unsigned char *ma, char *buffer);
static void insert_ip_addr(char *addr);
static void insert_mac_addr(char *addr);

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

    if(!device_name) device_name = strdup(DEFAULT_NET_DEVICE);

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

    // Initialize the netmon structure
    memset(&netmon, 0, sizeof(NETMON));
    netmon.rq = rate_queue_new(TIME_BLOCK_AMOUNT);
    netmon.tb = time_block_next(netmon.rq);
    netmon.ip_capacity = CHUNK;
    netmon.ip_addrs = (char **)malloc(netmon.ip_capacity * sizeof(char *));
    netmon.mac_capacity = CHUNK;
    netmon.mac_addrs = (char **)malloc(netmon.mac_capacity * sizeof(char *));

    return sockfd;
}

int netmon_mainloop(int sockfd, uint16_t mask)
{
    struct sockaddr_ll from;
    unsigned int addrlen;
    int len, rate_total;
    char buffer[4096];
    time_t current_time;

    ui_init();
    time_block_init(netmon.tb, time(NULL));
    rate_total = TIME_BLOCK_AMOUNT * TIME_BLOCK_LENGTH;

    for(;;) {

        // Process a packet if there is one
        len = recvfrom(sockfd, buffer, 4096, MSG_DONTWAIT, (struct sockaddr *)(&from), &addrlen);
        if(len > 0) {
            if(!skip_packet(buffer, mask)) {
                process_packet(buffer, len, mask);
                netmon.tb->byte_count += len;
            }
        }


        // Update volume / rate
        current_time = time(NULL) - netmon.tb->start_time;
        if(current_time >= TIME_BLOCK_LENGTH) {
            netmon.total_bytes += netmon.tb->byte_count;
            if(netmon.total_time < rate_total)
                netmon.total_time += TIME_BLOCK_LENGTH;
            netmon.tb = time_block_next(netmon.rq);
            netmon.total_bytes -= netmon.tb->byte_count;
            time_block_init(netmon.tb, time(NULL));
            ui_display_rate(netmon.total_bytes, netmon.total_time);
        }

        // Update packet numbers
        ui_display_ether_types(netmon.arp_total, netmon.ip4_total, netmon.ip6_total);
        ui_display_ip_types(netmon.tcp_total, netmon.udp_total, netmon.igmp_total, netmon.icmp_total);
        ui_display_arp_types(netmon.reply_total, netmon.request_total);
    }

    return 1;
}

static int skip_packet(char *packet_bytes, uint16_t mask)
{
    PACKET_ETH_HDR *eth_hdr;
    uint16_t type;

    eth_hdr = (PACKET_ETH_HDR *)packet_bytes;
    type = ntohs(eth_hdr->eth_type);
    return (mask != 0) && (mask != type);
}

static void process_packet(char *packet_bytes, int len, uint16_t mask)
{
    PACKET_ETH_HDR eth_hdr;
    char mac_src[MACLENGTH + 1];
    char mac_dest[MACLENGTH + 1];
    uint16_t type;

    memcpy(&eth_hdr, packet_bytes, sizeof(PACKET_ETH_HDR));
    mac_to_string(eth_hdr.eth_mac_src, mac_src);
    mac_to_string(eth_hdr.eth_mac_dest, mac_dest);
    insert_mac_addr(mac_src);
    insert_mac_addr(mac_dest);

    type = ntohs(eth_hdr.eth_type);
    switch(type) {
        case ETH_TYPE_IP4:
            process_ip4_packet(packet_bytes + sizeof(PACKET_ETH_HDR), mac_dest, mac_src);
            break;
        case ETH_TYPE_IP6:
            process_ip6_packet(packet_bytes + sizeof(PACKET_ETH_HDR), mac_dest, mac_src);
            break;
        case ETH_TYPE_ARP:
            process_arp_packet(packet_bytes + sizeof(PACKET_ETH_HDR), mac_dest, mac_src);
            break;
        default:
            sprintf(error_msg, "Unkown ethernet type: %04x", ntohs(eth_hdr.eth_type));
            ui_display_error(error_msg);
            break;
    }
}

static void process_ip4_packet(char *packet_bytes, char *mac_dest, char *mac_src)
{
    PACKET_IP4_HDR ip4_hdr;
    char ip4_src[IP4LENGTH + 1];
    char ip4_dest[IP4LENGTH + 1];

    memcpy(&ip4_hdr, packet_bytes, sizeof(PACKET_IP4_HDR));
    netmon.ip4_total++;
    switch(ip4_hdr.ip4_protocol) {
        case IP_PROTOCOL_ICMP:
            ui_display_packet(mac_dest, mac_src, "IPv4", "ICMP");
            netmon.icmp_total++;
            break;
        case IP_PROTOCOL_IGMP:
            ui_display_packet(mac_dest, mac_src, "IPv4", "IGMP");
            netmon.igmp_total++;
            break;
        case IP_PROTOCOL_TCP:
            ui_display_packet(mac_dest, mac_src, "IPv4", "TCP");
            netmon.tcp_total++;
            break;
        case IP_PROTOCOL_UDP:
            ui_display_packet(mac_dest, mac_src, "IPv4", "UDP");
            netmon.udp_total++;
            break;
        default:
            ui_display_packet(mac_dest, mac_src, "IPv4", "UNKNOWN");
            sprintf(error_msg, "Unkown IPv4 protocol: %02x", ip4_hdr.ip4_protocol);
            ui_display_error(error_msg);
            break;
    }

    ip4_to_string(ip4_hdr.ip4_src, ip4_src);
    ip4_to_string(ip4_hdr.ip4_dest, ip4_dest);
    insert_ip_addr(ip4_src);
    insert_ip_addr(ip4_dest);
}

static void process_ip6_packet(char *packet_bytes, char *mac_dest, char *mac_src)
{
    PACKET_IP6_HDR ip6_hdr;
    char ip6_src[IP6LENGTH + 1];
    char ip6_dest[IP6LENGTH + 1];

    memcpy(&ip6_hdr, packet_bytes, sizeof(PACKET_IP6_HDR));
    netmon.ip6_total++;
    switch(ip6_hdr.ip6_protocol) {
        case IP_PROTOCOL_IGMP:
            ui_display_packet(mac_dest, mac_src, "IPv6", "IGMP");
            netmon.igmp_total++;
            break;
        case IP_PROTOCOL_TCP:
            ui_display_packet(mac_dest, mac_src, "IPv6", "TCP");
            netmon.tcp_total++;
            break;
        case IP_PROTOCOL_UDP:
            ui_display_packet(mac_dest, mac_src, "IPv6", "UDP");
            netmon.udp_total++;
            break;
        case IP_PROTOCOL_IP6ICMP:
            ui_display_packet(mac_dest, mac_src, "IPv6", "ICMP");
            netmon.icmp_total++;
            break;
        default:
            ui_display_packet(mac_dest, mac_src, "IPv6", "UNKNOWN");
            sprintf(error_msg, "Unkown IPv6 protocol: %02x", ip6_hdr.ip6_protocol);
            ui_display_error(error_msg);
            break;
    }

    ip6_to_string(ip6_hdr.ip6_src, ip6_src);
    ip6_to_string(ip6_hdr.ip6_dest, ip6_dest);
    insert_ip_addr(ip6_src);
    insert_ip_addr(ip6_dest);
}

static void process_arp_packet(char *packet_bytes, char *mac_dest, char *mac_src)
{
    PACKET_ARP_HDR arp_hdr;

    memcpy(&arp_hdr, packet_bytes, sizeof(PACKET_ARP_HDR));
    netmon.arp_total++;
    switch(ntohs(arp_hdr.arp_oper)) {
        case ARP_OPER_REQUEST:
            ui_display_packet(mac_dest, mac_src, "ARP", "REQUEST");
            netmon.request_total++;
            break;
        case ARP_OPER_REPLY:
            ui_display_packet(mac_dest, mac_src, "ARP", "REPLY");
            netmon.reply_total++;
            break;
        default:
            ui_display_packet(mac_dest, mac_src, "ARP", "UNKNOWN");
            sprintf(error_msg, "Unkown ARP operation: %04x", ntohs(arp_hdr.arp_oper));
            ui_display_error(error_msg);
            break;
    }
}

static void ip6_to_string(unsigned short *ip, char *buffer)
{
    sprintf(buffer, "%01x:%01x:%01x:%01x:%01x:%01x:%01x:%01x",
            ntohs(ip[0]), ntohs(ip[1]), ntohs(ip[2]), ntohs(ip[3]), ntohs(ip[4]), ntohs(ip[5]), ntohs(ip[6]), ntohs(ip[7]));
    buffer[IP6LENGTH] = '\0';
}

static void ip4_to_string(unsigned char *ip, char *buffer)
{
    sprintf(buffer, "%d.%d.%d.%d",
            ip[0], ip[1], ip[2], ip[3]);
    buffer[IP4LENGTH] = '\0';
}

static void mac_to_string(unsigned char *ma, char *buffer)
{
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
            ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);
    buffer[MACLENGTH] = '\0';
}

static void insert_ip_addr(char *addr)
{
    for(int i = 0; i < netmon.ip_len; ++i)
        if(strcmp(netmon.ip_addrs[i], addr) == 0) return;
    if(netmon.ip_len >= netmon.ip_capacity - 1) {
        netmon.ip_capacity *= 2;
        netmon.ip_addrs = (char **)realloc(netmon.ip_addrs, netmon.ip_capacity * sizeof(char *));
    }
    netmon.ip_addrs[netmon.ip_len++] = strdup(addr);
    ui_display_ip_addr(addr);
}

static void insert_mac_addr(char *addr)
{
    for(int i = 0; i < netmon.mac_len; ++i)
        if(strcmp(netmon.mac_addrs[i], addr) == 0) return;
    if(netmon.mac_len >= netmon.mac_capacity - 1) {
        netmon.mac_capacity *= 2;
        netmon.mac_addrs = (char **)realloc(netmon.mac_addrs, netmon.mac_capacity * sizeof(char *));
    }
    netmon.mac_addrs[netmon.mac_len++] = strdup(addr);
    ui_display_mac_addr(addr);
}

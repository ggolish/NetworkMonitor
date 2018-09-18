#ifndef RATE_H_
#define RATE_H_

#include <time.h>

typedef struct {
    unsigned long byte_count; // The total byte count for the current block
    time_t start_time;        // The start time for the current block
    int arp_count;            // ARP packet count
    int ip4_count;            // IPv4 packet count
    int ip6_count;            // IPv6 packet count
    int reply_count;          // ARP reply packet count
    int request_count;        // ARP request packet count
    int igmp_count;           // IGMP packet count
    int icmp_count;           // ICMP packet count
    int tcp_count;            // TCP packet count
    int udp_count;            // UDP packet count
} TIME_BLOCK;

typedef struct {
    TIME_BLOCK **blocks;
    int capacity, pos;
} RATE_QUEUE;

extern RATE_QUEUE *rate_queue_new(int size);
extern TIME_BLOCK *time_block_new();
extern void time_block_init(TIME_BLOCK *tb, time_t start_time);
extern TIME_BLOCK *time_block_next(RATE_QUEUE *q);

#endif

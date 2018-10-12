#ifndef RATE_H_
#define RATE_H_

#include <time.h>

typedef struct {
    unsigned long byte_count; // The total byte count for the current block
    time_t start_time;        // The start time for the current block
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

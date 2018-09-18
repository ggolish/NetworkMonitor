#include "rate.h"

#include <stdlib.h>
#include <string.h>

RATE_QUEUE *rate_queue_new(int size)
{
    RATE_QUEUE *q;

    q = (RATE_QUEUE *)malloc(sizeof(RATE_QUEUE));
    q->blocks = (TIME_BLOCK **)malloc(size * sizeof(TIME_BLOCK *));
    for(int i = 0; i < size; ++i) q->blocks[i] = time_block_new();
    q->pos = 0;
    q->capacity = size;
    return q;
}

TIME_BLOCK *time_block_new()
{
    TIME_BLOCK *tb;

    tb = (TIME_BLOCK *)malloc(sizeof(TIME_BLOCK));
    return tb;
}

void time_block_init(TIME_BLOCK *tb, time_t start_time)
{
    memset(tb, 0, sizeof(TIME_BLOCK));
    tb->start_time = start_time;
}

TIME_BLOCK *time_block_next(RATE_QUEUE *q)
{
    TIME_BLOCK *tb;

    tb = q->blocks[q->pos++];
    if(q->pos >= q->capacity) q->pos = 0;
    return tb;
}

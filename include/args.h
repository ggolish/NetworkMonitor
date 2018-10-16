#ifndef NETMON_ARGS_H
#define NETMON_ARGS_H

#include <stdint.h>

typedef struct {
    char *net_device;
    uint16_t ether_type;
} netmon_args_t;

extern netmon_args_t *args_process(int argc, char *argv[]);

#endif

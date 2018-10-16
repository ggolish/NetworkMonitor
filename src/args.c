
#include "args.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static netmon_args_t *args_init();

netmon_args_t *args_process(int argc, char *argv[])
{
    netmon_args_t *args = args_init();
    int opt;

    while((opt = getopt(argc, argv, "d:t:")) != -1) {
        switch(opt) {
            case 'd':
                args->net_device = strdup(optarg);
                break;
            case 't':
                args->ether_type = strtol(optarg, NULL, 16);
                break;
            default:
                break;
        }
    }

    return args;
}

static netmon_args_t *args_init()
{
    netmon_args_t *args;

    args = (netmon_args_t *)malloc(sizeof(netmon_args_t));
    args->net_device = NULL;
    args->ether_type = 0;
    return args;
}


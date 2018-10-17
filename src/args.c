
#include "packet.h"
#include "args.h"
#include "errors.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ARG_DESCRIPTION 100
#define NUM_ARGS 3

static char arguments[][2][MAX_ARG_DESCRIPTION] = {
    {"-h", "Print out usage information"},
    {"-t <ethertype>", "Monitor packets of a specific ethertype, can be hexidecimal, 'ip4', 'ip6', 'arp', or 'netrans'"},
    {"-d <network-device>", "The name of the network device to monitor"}
};

static netmon_args_t *args_init();
static int parse_ethertype(netmon_args_t *args, char *arg);
static void usage(char *name);

netmon_args_t *args_process(int argc, char *argv[])
{
    netmon_args_t *args = args_init();
    int opt;

    while((opt = getopt(argc, argv, "d:t:h")) != -1) {
        switch(opt) {
            case 'd':
                args->net_device = strdup(optarg);
                break;
            case 't':
                if(parse_ethertype(args, optarg) == -1) {
                    sprintf(error_msg, "Invalid ethertype '%s'", optarg);
                    return NULL;
                }
                break;
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
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

static int parse_ethertype(netmon_args_t *args, char *arg)
{
    char *endptr;
    uint16_t type;

    type = strtol(arg, &endptr, 16);

    if(*endptr == '\0') {
        args->ether_type = type;
    } else {
        if(strcmp(arg, "ip4") == 0) {
            args->ether_type = ETH_TYPE_IP4;
        } else if(strcmp(arg, "ip6") == 0) {
            args->ether_type = ETH_TYPE_IP6;
        } else if(strcmp(arg, "arp") == 0) {
            args->ether_type = ETH_TYPE_ARP;
        } else if(strcmp(arg, "netrans") == 0) {
            args->ether_type = ETH_TYPE_NETRANS;
        } else {
            return -1;
        }
    }

    return 1;
}

static void usage(char *name)
{
    fprintf(stderr, "Usage: %s [-d <network device>] [-t <ethertype>]\n", name);
    for(int i = 0; i < NUM_ARGS; ++i) {
        fprintf(stderr, "%-20s %s\n", arguments[i][0], arguments[i][1]);
    }
}

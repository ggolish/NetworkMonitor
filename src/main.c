#include "netmon.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    int sockfd, opt;
    char *net_device = NULL;
    uint16_t ether_type = 0;

    while((opt = getopt(argc, argv, "d:t:")) != -1) {
        switch(opt) {
            case 'd':
                net_device = strdup(optarg);
                break;
            case 't':
                ether_type = strtol(optarg, NULL, 16);
                break;
            default:
                break;
        }
    }

    if((sockfd = netmon_init(net_device)) == -1) die(EXIT_FAILURE);

    netmon_mainloop(sockfd, ether_type);

    return EXIT_SUCCESS;
}

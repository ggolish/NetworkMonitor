#include "netmon.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int sockfd;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <device-name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if((sockfd = netmon_init(argv[1])) == -1) die(EXIT_FAILURE);

    return EXIT_SUCCESS;
}

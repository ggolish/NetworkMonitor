#include "netmon.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int sockfd;

    if((sockfd = netmon_init()) == -1) die(EXIT_FAILURE);

    return EXIT_SUCCESS;
}

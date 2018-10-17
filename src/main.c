#include "netmon.h"
#include "errors.h"
#include "args.h"

#include <stdlib.h>

int main(int argc, char *argv[])
{
    int sockfd;
    netmon_args_t *args;

    args = args_process(argc, argv);

    if(!args) {
        die(EXIT_FAILURE);
    }

    if((sockfd = netmon_init(args->net_device)) == -1) die(EXIT_FAILURE);

    netmon_mainloop(sockfd, args->ether_type);

    return EXIT_SUCCESS;
}

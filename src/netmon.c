#include "netmon.h"
#include "errors.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 

// Opens a raw socket and returns its file descriptor
int netmon_init()
{
    int sockfd;

    // Open a raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    
    if(sockfd == -1) {
        sprintf(error_msg, "Unable to open raw socket");
        return -1;
    }

    return sockfd;
}

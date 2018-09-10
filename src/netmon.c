#include "netmon.h"
#include "errors.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 

// Needed to check for device index
#include <sys/ioctl.h>
#include <net/if.h>

// Opens a raw socket and returns its file descriptor
int netmon_init(char *device_name)
{
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_ll sockaddr;

    // Open a raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    
    if(sockfd == -1) {
        sprintf(error_msg, "Unable to open raw socket (root privelidges required)");
        return -1;
    }

    // Ensure device_name is really a network device name and determine device index
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, device_name);
    if(ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        sprintf(error_msg, "Improper device name");
        return -1;
    }

    // Bind address to socket
    memset(&sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr.sll_family = AF_PACKET;
    sockaddr.sll_protocol = htons(ETH_P_ALL);
    sockaddr.sll_ifindex = ifr.ifr_ifindex;
    if(bind(sockfd, (struct sockaddr *)(&sockaddr), sizeof(struct sockaddr_ll)) == -1) {
        sprintf(error_msg, "Unable to bind address to socket");
        return -1;
    }

    return sockfd;
}

#ifndef NETMON_H_
#define NETMON_H_

// Opens a raw socket and returns its file descriptor
extern int netmon_init(char *device_name);

extern int netmon_mainloop(int sockfd);

#endif

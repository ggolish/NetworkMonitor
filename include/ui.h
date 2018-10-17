#ifndef UI_H_
#define UI_H_

extern void ui_init();
extern void ui_display_packet(char *mac_dest, char *mac_src, char *type, char *type_type);
extern void ui_display_mac_addr(char *addr);
extern void ui_display_ip_addr(char *addr);
extern void ui_display_ether_types(int arp, int ip4, int ip6, int netrans);
extern void ui_display_ip_types(int tcp, int udp, int igmp, int icmp);
extern void ui_display_arp_types(int reply, int request);
extern void ui_display_rate(int volume);
extern void ui_display_error(const char *error_msg);

#endif

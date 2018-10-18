#include "ui.h"

#include <ncurses.h>

#define MIN_STAT_DISPLAY 8
#define MIN_IP_SPACING 23
#define MIN_MAC_SPACING 20
#define MAX_MAC_SPACING_FACTOR 0.35
#define MIN_PROTOCOL_SPACING 10
#define MAX_PROTOCOL_SPACING_FACTOR 0.15

#define ETHER_TYPES_LINE   0
#define IP_TYPES_LINE      1
#define ARP_TYPES_LINE     2
#define NETRANS_TYPES_LINE 3
#define RATE_DISPLAY_LINE  4
#define ERROR_DISPLAY_LINE 5

#define K 1024

typedef struct {

    // Properties for the packet display window
    WINDOW *packet_display;
    int packet_display_width;
    int packet_spacing[2];
    int packet_lineno;

    // Properties for the MAC address display window
    WINDOW *mac_display;
    int mac_display_width;
    int mac_spacing;
    int mac_lineno;

    // Properties for the IP address display window
    WINDOW *ip_display;
    int ip_display_width;
    int ip_spacing;
    int ip_lineno;

} UI;

static UI ui;

static void calculate_spacing();
static void print_headers();

void ui_init()
{

    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    cbreak();
    noecho();
    curs_set(0);
    clear();
    calculate_spacing();
    print_headers();
    refresh();

    // Initializing packet display
    ui.packet_display = newwin(LINES - MIN_STAT_DISPLAY, 
            ui.packet_display_width, MIN_STAT_DISPLAY, 1);
    scrollok(ui.packet_display, true);
    wrefresh(ui.packet_display);
    ui.packet_lineno = 0;

    // Initializing mac address display
    ui.mac_display = newwin(LINES - MIN_STAT_DISPLAY - 2, ui.mac_display_width, 
            MIN_STAT_DISPLAY, ui.packet_display_width + 2);
    scrollok(ui.mac_display, true);
    wrefresh(ui.mac_display);
    ui.mac_lineno = 0;

    // Initializing mac address display
    ui.ip_display = newwin(LINES - MIN_STAT_DISPLAY - 2, ui.ip_display_width, 
            MIN_STAT_DISPLAY, ui.packet_display_width + ui.mac_display_width + 4);
    scrollok(ui.ip_display, true);
    wrefresh(ui.ip_display);
    ui.ip_lineno = 0;
}

void ui_display_packet(char *mac_dest, char *mac_src, char *type, char *type_type)
{
    wmove(ui.packet_display, ui.packet_lineno, 0);
    wprintw(ui.packet_display, "%-*s %-*s %-*s %-*s", 
           ui.packet_spacing[0], mac_dest, 
           ui.packet_spacing[0], mac_src, 
           ui.packet_spacing[1], type, 
           ui.packet_spacing[1], type_type);
    wrefresh(ui.packet_display);
    if(ui.packet_lineno == LINES - MIN_STAT_DISPLAY - 2) {
        scroll(ui.packet_display);
    } else {
        ui.packet_lineno++;
    }
        
}

void ui_display_ip_addr(char *addr)
{
    wmove(ui.ip_display, ui.ip_lineno, 1);
    wprintw(ui.ip_display, "%s", addr);
    wrefresh(ui.ip_display);
    if(ui.ip_lineno == LINES - MIN_STAT_DISPLAY - 3) {
        scroll(ui.ip_display);
    } else {
        ui.ip_lineno++;
    }
}

void ui_display_ether_types(int arp, int ip4, int ip6, int netrans)
{
    move(ETHER_TYPES_LINE, 1);
    clrtoeol();
    printw("ARP: %d    IPv4: %d    IPv6: %d    NETRANS: %d", arp, ip4, ip6, netrans);
    refresh();
}

void ui_display_ip_types(int tcp, int udp, int igmp, int icmp)
{
    move(IP_TYPES_LINE, 1);
    clrtoeol();
    printw("TCP: %d    UDP: %d    IGMP: %d    ICMP: %d", tcp, udp, igmp, icmp);
    refresh();
}

void ui_display_arp_types(int reply, int request)
{
    move(ARP_TYPES_LINE, 1);
    clrtoeol();
    printw("ARP Request: %d    ARP Reply: %d", request, reply);
    refresh();
}

void ui_display_netrans_types(int send_total, int receive_total, int ack_total, int chunk_total)
{
    move(NETRANS_TYPES_LINE, 1);
    clrtoeol();
    printw("NETRANS send: %d    NETRANS receive: %d    NETRANS ack: %d    NETRANS chunk: %d", send_total, receive_total, ack_total, chunk_total);
    refresh();
}

void ui_display_rate(int volume)
{
    double rate;

    rate = (double)volume;
    
    move(RATE_DISPLAY_LINE, 1);
    clrtoeol();

    if(rate > K) {
        rate /= K;
        if(rate > K) {
            rate /= K;
            if(rate > K) {
                rate /= K;
                printw("Rate: %6.02f  gb/s", rate);
            } else {
                printw("Rate: %6.02f  mb/s", rate);
            }
        } else {
            printw("Rate: %6.02f  kb/s", rate);
        }
    } else {
        printw("Rate: %6.02f  b/s", rate);
    }

    refresh();
}

void ui_display_mac_addr(char *addr)
{
    wmove(ui.mac_display, ui.mac_lineno, 1);
    wprintw(ui.mac_display, "%s", addr);
    wrefresh(ui.mac_display);
    if(ui.mac_lineno == LINES - MIN_STAT_DISPLAY - 3) {
        scroll(ui.mac_display);
    } else {
        ui.mac_lineno++;
    }
}

void ui_display_error(const char *error_msg)
{
    move(ERROR_DISPLAY_LINE, 1);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw(error_msg);
    attroff(COLOR_PAIR(2));
    refresh();
}

static void calculate_spacing()
{
    int x;

    // Calculate spacing for packet display
    x = (COLS - 1) * MAX_MAC_SPACING_FACTOR * 0.5;
    ui.packet_spacing[0] = (x > MIN_MAC_SPACING) ? x : MIN_MAC_SPACING;

    x = (COLS - 1) * MAX_PROTOCOL_SPACING_FACTOR * 0.5;
    ui.packet_spacing[1] = (x > MIN_PROTOCOL_SPACING) ? x : MIN_PROTOCOL_SPACING;

    ui.packet_display_width = ui.packet_spacing[0] * 2 + ui.packet_spacing[1] * 2 + 2;

    // Calculate spacing for MAC address display
    x = (COLS - ui.packet_display_width) * 0.4;
    ui.mac_spacing = (x > MIN_MAC_SPACING) ? x : MIN_MAC_SPACING;
    ui.mac_display_width = ui.mac_spacing;

    // Calculate spacing for IP address display
    x = COLS - ui.packet_display_width - ui.mac_display_width - 5;
    ui.ip_spacing = (x > MIN_IP_SPACING) ? x : MIN_IP_SPACING;
    ui.ip_display_width = ui.ip_spacing;
}

static void print_headers()
{
    int hoffset = 0;

    attron(COLOR_PAIR(1));

    // Printing packet display header
    move(MIN_STAT_DISPLAY - 1, hoffset);
    printw(" %-*s %-*s %-*s %-*s", 
            ui.packet_spacing[0], "MAC Destination", 
            ui.packet_spacing[0], "MAC Source", 
            ui.packet_spacing[1], "Protocol", 
            ui.packet_spacing[1] - 1, "Type");
    hoffset += ui.packet_display_width + 1;
    move(MIN_STAT_DISPLAY - 1, hoffset);
    vline(' ', LINES - MIN_STAT_DISPLAY);

    // Printing MAC address display header
    hoffset++;
    move(MIN_STAT_DISPLAY - 1, hoffset);
    printw(" %-*s", ui.mac_spacing, "MAC Addresses");
    hoffset += ui.mac_display_width + 1;
    move(MIN_STAT_DISPLAY - 1, hoffset);
    vline(' ', LINES - MIN_STAT_DISPLAY);

    // Printing MAC address display header
    hoffset++;
    move(MIN_STAT_DISPLAY - 1, hoffset);
    printw(" %-*s", ui.ip_spacing, "IP Addresses");

    attroff(COLOR_PAIR(1));
}

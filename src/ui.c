#include "ui.h"

#include <ncurses.h>

#define MIN_STAT_DISPLAY 10
#define MIN_MAC_SPACING 20
#define MIN_PROTOCOL_SPACING 10

typedef struct {
    WINDOW *packet_display;
    int packet_display_width;
    int packet_spacing[2];
    int lineno;
} UI;

static UI ui;

static void calculate_spacing();
static void print_packet_header();

void ui_init()
{

    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    cbreak();
    noecho();
    curs_set(0);
    clear();
    calculate_spacing();
    print_packet_header();
    refresh();

    ui.packet_display = newwin(LINES - MIN_STAT_DISPLAY, ui.packet_display_width, MIN_STAT_DISPLAY, 1);
    scrollok(ui.packet_display, true);
    wrefresh(ui.packet_display);
    ui.lineno = 0;
}

void ui_display_packet(char *mac_dest, char *mac_src, char *type, char *type_type)
{
    wmove(ui.packet_display, ui.lineno, 0);
    wprintw(ui.packet_display, "%-*s %-*s %-*s %-*s", 
           ui.packet_spacing[0], mac_dest, 
           ui.packet_spacing[0], mac_src, 
           ui.packet_spacing[1], type, 
           ui.packet_spacing[1], type_type);
    wrefresh(ui.packet_display);
    if(ui.lineno == LINES - MIN_STAT_DISPLAY - 2) {
        scroll(ui.packet_display);
    } else {
        ui.lineno++;
    }
        
}

static void calculate_spacing()
{
    int x;

    x = (COLS - 1) * 35 / 100 * 50 / 100;
    ui.packet_spacing[0] = (x > MIN_MAC_SPACING) ? x : MIN_MAC_SPACING;

    x = (COLS - 1) * 15 / 100 * 50 / 100;
    ui.packet_spacing[1] = (x > MIN_PROTOCOL_SPACING) ? x : MIN_PROTOCOL_SPACING;

    ui.packet_display_width = ui.packet_spacing[0] * 2 + ui.packet_spacing[1] * 2 + 2;
}

static void print_packet_header()
{
    attron(COLOR_PAIR(1));
    move(MIN_STAT_DISPLAY - 1, 0);
    printw(" %-*s %-*s %-*s %-*s", 
            ui.packet_spacing[0], "MAC Destination", 
            ui.packet_spacing[0], "MAC Source", 
            ui.packet_spacing[1], "Protocol", 
            ui.packet_spacing[1] - 1, "Type");
    move(MIN_STAT_DISPLAY - 1, ui.packet_display_width + 1);
    vline(' ', LINES - MIN_STAT_DISPLAY);
}

#include "ui.h"

#include <ncurses.h>

#define MIN_STAT_DISPLAY 10

typedef struct {
    WINDOW *packet_display;
    int lineno;
} UI;

static UI ui;

void ui_init()
{

    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    cbreak();
    noecho();
    curs_set(0);
    clear();
    attron(COLOR_PAIR(1));
    move(MIN_STAT_DISPLAY - 1, 0);
    printw("%-17s    %-17s    %-8s    %-7s", 
            "MAC Destination", "MAC Source", "Protocol", "Type");
    refresh();

    ui.packet_display = newwin(LINES - MIN_STAT_DISPLAY, COLS, MIN_STAT_DISPLAY, 0);
    scrollok(ui.packet_display, true);
    wrefresh(ui.packet_display);
    ui.lineno = 0;
}

void ui_display_packet(char *mac_dest, char *mac_src, char *type, char *type_type)
{
    wmove(ui.packet_display, ui.lineno, 0);
    wprintw(ui.packet_display, "%17s    %17s    %8s    %7s", mac_dest, mac_src, type, type_type);
    wrefresh(ui.packet_display);
    if(ui.lineno == LINES - MIN_STAT_DISPLAY - 1) {
        scroll(ui.packet_display);
    } else {
        ui.lineno++;
    }
        
}

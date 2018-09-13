#include "ui.h"

#include <ncurses.h>

typedef struct {
    int lineno;
} UI;

static UI ui;

void ui_init()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    clear();
    refresh();

    ui.lineno = 0;
}

void ui_display_packet(char *mac_dest, char *mac_src, char *type, char *type_type)
{
    move(ui.lineno++, 0);
    printw("%s %s %s %s", mac_dest, mac_src, type, type_type);
}

#include "ui.h"

#include <ncurses.h>

void ui_init()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    clear();
    refresh();
}

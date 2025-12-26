#include "crt.h"

void CRT_init(void) {
   initscr();
   cbreak();
   noecho();
   keypad(stdscr, true);
   curs_set(0);
   start_color();
   use_default_colors();

   init_pair(COLOR_HEADER_FOCUS, COLOR_BLACK, COLOR_CYAN);
   init_pair(COLOR_HEADER_UNFOCUS, COLOR_WHITE, COLOR_BLUE);
   init_pair(COLOR_SELECTION_FOCUS, COLOR_BLACK, COLOR_GREEN);
   init_pair(COLOR_SELECTION_UNFOCUS, COLOR_BLACK, COLOR_WHITE);
}

void CRT_shutdown(void) {
   endwin();
}

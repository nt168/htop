#define _XOPEN_SOURCE 700

#include "crt.h"

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void CRT_init(void) {
   setlocale(LC_ALL, "");
   initscr();
   cbreak();
   noecho();
   keypad(stdscr, true);
   curs_set(0);
   start_color();
   use_default_colors();

   init_pair(COLOR_HEADER_FOCUS, COLOR_BLACK, COLOR_GREEN);
   init_pair(COLOR_HEADER_UNFOCUS, COLOR_WHITE, COLOR_GREEN);
   init_pair(COLOR_SELECTION_FOCUS, COLOR_BLACK, COLOR_CYAN);
   init_pair(COLOR_SELECTION_UNFOCUS, COLOR_BLACK, COLOR_WHITE);
}

void CRT_shutdown(void) {
   endwin();
}

void CRT_drawString(int y, int x, int width, const char* text) {
   if (!text || width <= 0) {
      return;
   }

   if (*text == '\0') {
      return;
   }

   mbstate_t state;
   memset(&state, 0, sizeof(state));

   const char* cursor = text;
   size_t bytes_to_draw = 0;
   int used_cols = 0;

   while (*cursor && used_cols < width) {
      wchar_t wc = L'\0';
      size_t result = mbrtowc(&wc, cursor, MB_CUR_MAX, &state);
      if (result == (size_t)-1 || result == (size_t)-2) {
         memset(&state, 0, sizeof(state));
         result = 1;
         wc = L'?';
      } else if (result == 0) {
         break;
      }

      int char_width = wcwidth(wc);
      if (char_width < 0) {
         char_width = 1;
      }
      if (used_cols + char_width > width) {
         break;
      }

      cursor += result;
      bytes_to_draw += result;
      used_cols += char_width;
   }

   if (bytes_to_draw > 0) {
      mvaddnstr(y, x, text, (int)bytes_to_draw);
   }
}

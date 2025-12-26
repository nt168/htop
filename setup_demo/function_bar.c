#include "function_bar.h"

#include <ncurses.h>
#include <string.h>

void FunctionBar_draw(int y, int width, const char* text) {
   if (!text) {
      return;
   }
   mvhline(y, 0, ' ', width);
   mvaddnstr(y, 1, text, width - 2);
}

#include "function_bar.h"

#include <ncurses.h>

#include "crt.h"

void FunctionBar_draw(int y, int width, const char* text) {
   if (!text) {
      return;
   }
   mvhline(y, 0, ' ', width);
   CRT_drawString(y, 1, width - 2, text);
}

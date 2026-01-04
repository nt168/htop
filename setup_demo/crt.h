#ifndef SETUP_DEMO_CRT_H
#define SETUP_DEMO_CRT_H

#include <ncurses.h>

typedef enum {
   COLOR_HEADER_FOCUS = 1,
   COLOR_HEADER_UNFOCUS,
   COLOR_SELECTION_FOCUS,
   COLOR_SELECTION_UNFOCUS,
} ColorPair;

void CRT_init(void);
void CRT_shutdown(void);
void CRT_drawString(int y, int x, int width, const char* text);

#endif

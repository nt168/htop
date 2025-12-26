#include "panel.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "crt.h"

Panel* Panel_new(int x, int y, int w, int h, const char* header) {
   Panel* panel = calloc(1, sizeof(Panel));
   if (!panel) {
      exit(EXIT_FAILURE);
   }
   panel->x = x;
   panel->y = y;
   panel->w = w;
   panel->h = h;
   panel->header = header;
   panel->items = Vector_new(8);
   panel->needs_redraw = true;
   return panel;
}

void Panel_delete(Panel* panel) {
   if (!panel) {
      return;
   }
   for (size_t i = 0; i < Vector_size(panel->items); i++) {
      ListItem_delete(Vector_get(panel->items, i));
   }
   Vector_delete(panel->items);
   free(panel);
}

void Panel_setHeader(Panel* panel, const char* header) {
   if (!panel) {
      return;
   }
   panel->header = header;
   panel->needs_redraw = true;
}

void Panel_setGeometry(Panel* panel, int x, int y, int w, int h) {
   panel->x = x;
   panel->y = y;
   panel->w = w;
   panel->h = h;
   panel->needs_redraw = true;
}

void Panel_clear(Panel* panel) {
   if (!panel) {
      return;
   }
   for (size_t i = 0; i < Vector_size(panel->items); i++) {
      ListItem_delete(Vector_get(panel->items, i));
   }
   Vector_clear(panel->items);
   panel->selected = 0;
   panel->scroll = 0;
   panel->needs_redraw = true;
}

void Panel_addItem(Panel* panel, const char* text) {
   if (!panel) {
      return;
   }
   Vector_add(panel->items, ListItem_new(text));
   panel->needs_redraw = true;
}

ListItem* Panel_getSelectedItem(const Panel* panel) {
   if (!panel || Vector_size(panel->items) == 0) {
      return NULL;
   }
   return Vector_get(panel->items, (size_t)panel->selected);
}

int Panel_getSelectedIndex(const Panel* panel) {
   return panel ? panel->selected : 0;
}

void Panel_setSelected(Panel* panel, int index) {
   if (!panel) {
      return;
   }
   int size = (int)Vector_size(panel->items);
   if (size == 0) {
      panel->selected = 0;
      return;
   }
   if (index < 0) {
      index = 0;
   }
   if (index >= size) {
      index = size - 1;
   }
   panel->selected = index;
   panel->needs_redraw = true;
}

static void Panel_adjustScroll(Panel* panel) {
   int visible = panel->h - 1;
   if (panel->selected < panel->scroll) {
      panel->scroll = panel->selected;
   } else if (panel->selected >= panel->scroll + visible) {
      panel->scroll = panel->selected - visible + 1;
   }
   if (panel->scroll < 0) {
      panel->scroll = 0;
   }
}

bool Panel_handleKey(Panel* panel, int key) {
   if (!panel) {
      return false;
   }
   int size = (int)Vector_size(panel->items);
   int old = panel->selected;
   switch (key) {
      case KEY_UP:
         if (panel->selected > 0) {
            panel->selected--;
         }
         break;
      case KEY_DOWN:
         if (panel->selected + 1 < size) {
            panel->selected++;
         }
         break;
      case KEY_HOME:
         panel->selected = 0;
         break;
      case KEY_END:
         if (size > 0) {
            panel->selected = size - 1;
         }
         break;
      default:
         return false;
   }

   Panel_adjustScroll(panel);
   if (old != panel->selected) {
      panel->needs_redraw = true;
      return true;
   }
   return false;
}

void Panel_draw(Panel* panel, bool force) {
   if (!panel || (!panel->needs_redraw && !force)) {
      return;
   }

   int header_attr = panel->has_focus ? COLOR_PAIR(COLOR_HEADER_FOCUS)
                                      : COLOR_PAIR(COLOR_HEADER_UNFOCUS);
   int selection_attr = panel->has_focus ? COLOR_PAIR(COLOR_SELECTION_FOCUS)
                                         : COLOR_PAIR(COLOR_SELECTION_UNFOCUS);

   attrset(header_attr);
   mvhline(panel->y, panel->x, ' ', panel->w);
   if (panel->header) {
      mvaddnstr(panel->y, panel->x + 1, panel->header, panel->w - 2);
   }

   int list_y = panel->y + 1;
   int visible = panel->h - 1;
   size_t size = Vector_size(panel->items);
   for (int row = 0; row < visible; row++) {
      int idx = panel->scroll + row;
      mvhline(list_y + row, panel->x, ' ', panel->w);
      if (idx >= (int)size) {
         continue;
      }
      ListItem* item = Vector_get(panel->items, (size_t)idx);
      if (!item) {
         continue;
      }
      if (idx == panel->selected) {
         attrset(selection_attr);
         mvhline(list_y + row, panel->x, ' ', panel->w);
      } else {
         attrset(A_NORMAL);
      }
      mvaddnstr(list_y + row, panel->x + 1, item->text, panel->w - 2);
   }

   attrset(A_NORMAL);
   panel->needs_redraw = false;
}

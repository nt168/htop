#ifndef SETUP_DEMO_PANEL_H
#define SETUP_DEMO_PANEL_H

#include <stdbool.h>

#include "list_item.h"
#include "vector.h"

typedef struct Panel {
   int x;
   int y;
   int w;
   int h;
   int selected;
   int scroll;
   bool needs_redraw;
   bool has_focus;
   const char* header;
   Vector* items;
} Panel;

Panel* Panel_new(int x, int y, int w, int h, const char* header);
void Panel_delete(Panel* panel);
void Panel_setHeader(Panel* panel, const char* header);
void Panel_setGeometry(Panel* panel, int x, int y, int w, int h);
void Panel_clear(Panel* panel);
void Panel_addItem(Panel* panel, const char* text);
ListItem* Panel_getSelectedItem(const Panel* panel);
int Panel_getSelectedIndex(const Panel* panel);
bool Panel_handleKey(Panel* panel, int key);
void Panel_setSelected(Panel* panel, int index);
void Panel_draw(Panel* panel, bool force);

#endif

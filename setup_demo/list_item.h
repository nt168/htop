#ifndef SETUP_DEMO_LIST_ITEM_H
#define SETUP_DEMO_LIST_ITEM_H

typedef struct ListItem {
   char* text;
} ListItem;

ListItem* ListItem_new(const char* text);
void ListItem_delete(ListItem* item);

#endif

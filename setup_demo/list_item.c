#include "list_item.h"

#include <stdlib.h>
#include <string.h>

static char* xstrdup(const char* text) {
   size_t len = strlen(text);
   char* copy = malloc(len + 1);
   if (!copy) {
      exit(EXIT_FAILURE);
   }
   memcpy(copy, text, len + 1);
   return copy;
}

ListItem* ListItem_new(const char* text) {
   ListItem* item = calloc(1, sizeof(ListItem));
   if (!item) {
      exit(EXIT_FAILURE);
   }
   item->text = xstrdup(text ? text : "");
   return item;
}

void ListItem_delete(ListItem* item) {
   if (!item) {
      return;
   }
   free(item->text);
   free(item);
}

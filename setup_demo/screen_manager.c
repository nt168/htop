#include "screen_manager.h"

#include <stdlib.h>

ScreenManager* ScreenManager_new(void) {
   ScreenManager* manager = calloc(1, sizeof(ScreenManager));
   if (!manager) {
      exit(EXIT_FAILURE);
   }
   manager->capacity = 4;
   manager->panels = calloc(manager->capacity, sizeof(Panel*));
   if (!manager->panels) {
      exit(EXIT_FAILURE);
   }
   return manager;
}

void ScreenManager_delete(ScreenManager* manager) {
   if (!manager) {
      return;
   }
   free(manager->panels);
   free(manager);
}

void ScreenManager_add(ScreenManager* manager, Panel* panel) {
   if (!manager || !panel) {
      return;
   }
   if (manager->count >= manager->capacity) {
      manager->capacity *= 2;
      Panel** panels = realloc(manager->panels, manager->capacity * sizeof(Panel*));
      if (!panels) {
         exit(EXIT_FAILURE);
      }
      manager->panels = panels;
   }
   manager->panels[manager->count++] = panel;
}

Panel* ScreenManager_getFocused(const ScreenManager* manager) {
   if (!manager || manager->count == 0) {
      return NULL;
   }
   return manager->panels[manager->focus];
}

void ScreenManager_focusNext(ScreenManager* manager) {
   if (!manager || manager->count == 0) {
      return;
   }
   manager->focus = (manager->focus + 1) % manager->count;
}

void ScreenManager_draw(ScreenManager* manager, bool force) {
   if (!manager) {
      return;
   }
   for (size_t i = 0; i < manager->count; i++) {
      manager->panels[i]->has_focus = (i == manager->focus);
      Panel_draw(manager->panels[i], force);
   }
}

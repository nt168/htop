#ifndef SETUP_DEMO_SCREEN_MANAGER_H
#define SETUP_DEMO_SCREEN_MANAGER_H

#include <stddef.h>

#include "panel.h"

typedef struct ScreenManager {
   Panel** panels;
   size_t count;
   size_t capacity;
   size_t focus;
} ScreenManager;

ScreenManager* ScreenManager_new(void);
void ScreenManager_delete(ScreenManager* manager);
void ScreenManager_add(ScreenManager* manager, Panel* panel);
Panel* ScreenManager_getFocused(const ScreenManager* manager);
void ScreenManager_focusNext(ScreenManager* manager);
void ScreenManager_draw(ScreenManager* manager, bool force);

#endif

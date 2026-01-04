#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>

#include "crt.h"
#include "function_bar.h"
#include "panel.h"
#include "screen_manager.h"
#include "setup_config.h"

static void populate_functions(Panel* panel, const ConfigCategory* category) {
   Panel_clear(panel);
   for (size_t i = 0; i < Config_functionCount(category); i++) {
      const ConfigFunction* function = Config_getFunction(category, i);
      if (function) {
         Panel_addItem(panel, function->name);
      }
   }
   Panel_setSelected(panel, 0);
}

static void populate_subitems(Panel* panel, const ConfigFunction* function) {
   Panel_clear(panel);
   for (size_t i = 0; i < Config_subitemCount(function); i++) {
      const ConfigSubItem* subitem = Config_getSubItem(function, i);
      if (subitem) {
         Panel_addItem(panel, subitem->name);
      }
   }
   Panel_setSelected(panel, 0);
}

static void populate_details(Panel* panel, const Vector* details) {
   Panel_clear(panel);
   if (!details) {
      return;
   }
   for (size_t i = 0; i < Vector_size(details); i++) {
      const char* line = Vector_get(details, i);
      if (line) {
         Panel_addItem(panel, line);
      }
   }
   Panel_setSelected(panel, 0);
}

int main(void) 
{
   const char* config_path = "config.txt";
   Config* config = Config_load(config_path);
   if (!config || Config_categoryCount(config) == 0) {
      return 1;
   }

   CRT_init();

   int height = LINES - 2;
   int width = COLS;

   int left_w = 16;
   int mid_w = 16;
   int sub_w = 16;
   int right_w = width - left_w - mid_w - sub_w - 3;

   Panel* left = Panel_new(0, 0, left_w, height, "列表");
   Panel* middle = Panel_new(left_w + 1, 0, mid_w, height, "功能");
   Panel* subitems = Panel_new(left_w + mid_w + 2, 0, sub_w, height, "子项");
   Panel* right = Panel_new(left_w + mid_w + sub_w + 3, 0, right_w, height, "执行参数");

   for (size_t i = 0; i < Config_categoryCount(config); i++) {
      const ConfigCategory* category = Config_getCategory(config, i);
      if (category) {
         Panel_addItem(left, category->name);
      }
   }

   const ConfigCategory* category = Config_getCategory(config, 0);
   populate_functions(middle, category);
   const ConfigFunction* function = Config_getFunction(category, 0);
   if (function && Config_subitemCount(function) > 0) {
      populate_subitems(subitems, function);
      const ConfigSubItem* subitem = Config_getSubItem(function, 0);
      if (subitem) {
         populate_details(right, subitem->details);
      }
   } else if (function) {
      populate_details(right, function->details);
   }

   ScreenManager* manager = ScreenManager_new();
   ScreenManager_add(manager, left);
   ScreenManager_add(manager, middle);
   ScreenManager_add(manager, subitems);
   ScreenManager_add(manager, right);

   bool running = true;
   int last_left = Panel_getSelectedIndex(left);
   int last_middle = Panel_getSelectedIndex(middle);
   int last_sub = Panel_getSelectedIndex(subitems);
   bool showing_subitems = false;

   if (function && Config_subitemCount(function) > 0) {
      showing_subitems = true;
   }

   if (!showing_subitems) {
      Panel_setGeometry(subitems, left_w + mid_w + 2, 0, 0, height);
      Panel_setGeometry(right, left_w + mid_w + 2, 0, width - left_w - mid_w - 2, height);
   }
   ScreenManager_ensureFocusVisible(manager);

   while (running) {
      ScreenManager_draw(manager, false);
      FunctionBar_draw(LINES - 1, COLS, "Tab/\u2190/\u2192: Switch  \u2191/\u2193: Move  q: Quit");
      refresh();

      int ch = getch();
      Panel* focus = ScreenManager_getFocused(manager);

      if (ch == 'q' || ch == 'Q') {
         running = false;
         break;
      }
      if (ch == '\t' || ch == KEY_RIGHT) {
         ScreenManager_focusNext(manager);
         ScreenManager_draw(manager, true);
         continue;
      }
      if (ch == KEY_LEFT) {
         ScreenManager_focusPrev(manager);
         ScreenManager_draw(manager, true);
         continue;
      }

      if (focus) {
         Panel_handleKey(focus, ch);
      }

      int current_left = Panel_getSelectedIndex(left);
      int current_middle = Panel_getSelectedIndex(middle);
      bool left_changed = current_left != last_left;

      if (left_changed) {
         const ConfigCategory* active_category = Config_getCategory(config, (size_t)current_left);
         populate_functions(middle, active_category);
         last_left = current_left;
         last_middle = 0;
         last_sub = 0;
         current_middle = Panel_getSelectedIndex(middle);
      }

      bool middle_changed = left_changed || current_middle != last_middle;
      if (middle_changed) {
         const ConfigCategory* active_category = Config_getCategory(config, (size_t)current_left);
         const ConfigFunction* active_function = Config_getFunction(active_category, (size_t)current_middle);
         bool has_subitems = active_function && Config_subitemCount(active_function) > 0;
         if (has_subitems) {
            populate_subitems(subitems, active_function);
         } else {
            Panel_clear(subitems);
         }
         if (has_subitems != showing_subitems) {
            if (has_subitems) {
               right_w = width - left_w - mid_w - sub_w - 3;
               Panel_setGeometry(subitems, left_w + mid_w + 2, 0, sub_w, height);
               Panel_setGeometry(right, left_w + mid_w + sub_w + 3, 0, right_w, height);
            } else {
               Panel_setGeometry(subitems, left_w + mid_w + 2, 0, 0, height);
               Panel_setGeometry(right, left_w + mid_w + 2, 0, width - left_w - mid_w - 2, height);
            }
            showing_subitems = has_subitems;
            ScreenManager_ensureFocusVisible(manager);
         }
         if (has_subitems) {
            const ConfigSubItem* active_subitem = Config_getSubItem(active_function, 0);
            if (active_subitem) {
               populate_details(right, active_subitem->details);
            } else {
               Panel_clear(right);
            }
         } else if (active_function) {
            populate_details(right, active_function->details);
         } else {
            Panel_clear(right);
         }
         last_middle = current_middle;
         last_sub = 0;
      } else if (showing_subitems) {
         int current_sub = Panel_getSelectedIndex(subitems);
         if (current_sub != last_sub) {
            const ConfigCategory* active_category = Config_getCategory(config, (size_t)current_left);
            const ConfigFunction* active_function = Config_getFunction(active_category, (size_t)current_middle);
            const ConfigSubItem* active_subitem = Config_getSubItem(active_function, (size_t)current_sub);
            if (active_subitem) {
               populate_details(right, active_subitem->details);
            }
            last_sub = current_sub;
         }
      }
   }

   ScreenManager_delete(manager);
   Panel_delete(left);
   Panel_delete(middle);
   Panel_delete(subitems);
   Panel_delete(right);
   Config_delete(config);
   CRT_shutdown();
   return 0;
}

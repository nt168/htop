#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>

#include "crt.h"
#include "function_bar.h"
#include "panel.h"
#include "screen_manager.h"
#include "config.h"

typedef struct {
   const char* name;
   const char* const* details;
   size_t detail_count;
} ItemDefinition;

typedef struct {
   const char* name;
   const ItemDefinition* items;
   size_t item_count;
} CategoryDefinition;

static const char* const cpu_details[] = {"usage", "freq", "cache"};
static const char* const topdown_details[] = {"stage1", "stage2"};
static const char* const pcie_tx_details[] = {"pkt/s", "Gbps"};
static const char* const pcie_rx_details[] = {"pkt/s", "Gbps"};
static const char* const ddr_bw_details[] = {"read", "write", "total"};

static const ItemDefinition compute_items[] = {
   {"cpu", cpu_details, sizeof(cpu_details) / sizeof(cpu_details[0])},
   {"topdown", topdown_details, sizeof(topdown_details) / sizeof(topdown_details[0])},
};

static const ItemDefinition pcie_items[] = {
   {"tx", pcie_tx_details, sizeof(pcie_tx_details) / sizeof(pcie_tx_details[0])},
   {"rx", pcie_rx_details, sizeof(pcie_rx_details) / sizeof(pcie_rx_details[0])},
};

static const ItemDefinition ddr_items[] = {
   {"bandwidth", ddr_bw_details, sizeof(ddr_bw_details) / sizeof(ddr_bw_details[0])},
};

static const CategoryDefinition categories[] = {
   {"计算处理分析", compute_items, sizeof(compute_items) / sizeof(compute_items[0])},
   {"pcie接口性能", pcie_items, sizeof(pcie_items) / sizeof(pcie_items[0])},
   {"ddr接口性能", ddr_items, sizeof(ddr_items) / sizeof(ddr_items[0])},
};

static void populate_items(Panel* panel, const CategoryDefinition* category) {
   Panel_clear(panel);
   for (size_t i = 0; i < category->item_count; i++) {
      Panel_addItem(panel, category->items[i].name);
   }
   Panel_setSelected(panel, 0);
}

static void populate_details(Panel* panel, const ItemDefinition* item) {
   Panel_clear(panel);
   for (size_t i = 0; i < item->detail_count; i++) {
      Panel_addItem(panel, item->details[i]);
   }
   Panel_setSelected(panel, 0);
}

int main(void) 
{

   // printf("%s\n", topdown);
   // return 0;
   CRT_init();

   int height = LINES - 2;
   int width = COLS;

   int left_w = 16;
   int mid_w = 16;
   int right_w = width - left_w - mid_w - 2;

   Panel* left = Panel_new(0, 0, left_w, height, "列表");
   Panel* middle = Panel_new(left_w + 1, 0, mid_w, height, "Options");
   Panel* right = Panel_new(left_w + mid_w + 2, 0, right_w, height, "Details");

   for (size_t i = 0; i < sizeof(categories) / sizeof(categories[0]); i++) {
      Panel_addItem(left, categories[i].name);
   }

   populate_items(middle, &categories[0]);
   populate_details(right, &categories[0].items[0]);

   ScreenManager* manager = ScreenManager_new();
   ScreenManager_add(manager, left);
   ScreenManager_add(manager, middle);
   ScreenManager_add(manager, right);

   bool running = true;
   int last_left = Panel_getSelectedIndex(left);
   int last_middle = Panel_getSelectedIndex(middle);

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

      if (current_left != last_left) {
         const CategoryDefinition* category = &categories[current_left];
         populate_items(middle, category);
         populate_details(right, &category->items[0]);
         last_left = current_left;
         last_middle = 0;
      } else if (current_middle != last_middle) {
         const CategoryDefinition* category = &categories[current_left];
         populate_details(right, &category->items[current_middle]);
         last_middle = current_middle;
      }
   }

   ScreenManager_delete(manager);
   Panel_delete(left);
   Panel_delete(middle);
   Panel_delete(right);
   CRT_shutdown();
   return 0;
}

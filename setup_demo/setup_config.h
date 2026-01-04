#ifndef SETUP_DEMO_SETUP_CONFIG_H
#define SETUP_DEMO_SETUP_CONFIG_H

#include <stddef.h>

#include "vector.h"

typedef struct ConfigSubItem {
   char* name;
   Vector* details;
} ConfigSubItem;

typedef struct ConfigFunction {
   char* name;
   Vector* subitems;
   Vector* details;
} ConfigFunction;

typedef struct ConfigCategory {
   char* name;
   Vector* functions;
} ConfigCategory;

typedef struct Config {
   Vector* categories;
} Config;

Config* Config_load(const char* path);
void Config_delete(Config* config);
size_t Config_categoryCount(const Config* config);
const ConfigCategory* Config_getCategory(const Config* config, size_t index);
size_t Config_functionCount(const ConfigCategory* category);
const ConfigFunction* Config_getFunction(const ConfigCategory* category, size_t index);
size_t Config_subitemCount(const ConfigFunction* function);
const ConfigSubItem* Config_getSubItem(const ConfigFunction* function, size_t index);

#endif

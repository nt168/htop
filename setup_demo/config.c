#include "setup_config.h"

#include <stdbool.h>
#include <stdio.h>
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

static char* trim_whitespace(char* text) {
   while (*text == ' ' || *text == '\t') {
      text++;
   }
   size_t len = strlen(text);
   while (len > 0 && (text[len - 1] == ' ' || text[len - 1] == '\t')) {
      text[len - 1] = '\0';
      len--;
   }
   return text;
}

static int count_indent(const char* line, const char** start) {
   int tabs = 0;
   int spaces = 0;
   const char* cursor = line;
   while (*cursor == '\t' || *cursor == ' ') {
      if (*cursor == '\t') {
         tabs++;
      } else {
         spaces++;
      }
      cursor++;
   }
   *start = cursor;
   int indent = tabs;
   if (spaces > 0) {
      indent += spaces / 2;
      if (spaces % 2 != 0) {
         indent++;
      }
   }
   return indent;
}

static bool is_separator_line(const char* text) {
   size_t len = strlen(text);
   if (len < 3) {
      return false;
   }
   for (size_t i = 0; i < len; i++) {
      if (text[i] != '-') {
         return false;
      }
   }
   return true;
}

static bool is_header_line(const char* text) {
   return strstr(text, "功能") && strstr(text, "执行参数");
}

static char* replace_tabs(const char* text) {
   size_t len = strlen(text);
   char* out = malloc(len + 1);
   if (!out) {
      exit(EXIT_FAILURE);
   }
   for (size_t i = 0; i < len; i++) {
      out[i] = (text[i] == '\t') ? ' ' : text[i];
   }
   out[len] = '\0';
   return out;
}

static void add_detail_line(Vector* details, const char* text, int extra_indent) {
   if (!details || !text) {
      return;
   }
   char* normalized = replace_tabs(text);
   size_t prefix = extra_indent > 0 ? (size_t)extra_indent * 2 : 0;
   size_t len = strlen(normalized);
   char* line = malloc(prefix + len + 1);
   if (!line) {
      exit(EXIT_FAILURE);
   }
   if (prefix > 0) {
      memset(line, ' ', prefix);
   }
   memcpy(line + prefix, normalized, len + 1);
   free(normalized);
   Vector_add(details, line);
}

static ConfigCategory* ConfigCategory_new(const char* name) {
   ConfigCategory* category = calloc(1, sizeof(ConfigCategory));
   if (!category) {
      exit(EXIT_FAILURE);
   }
   category->name = xstrdup(name);
   category->functions = Vector_new(4);
   return category;
}

static ConfigFunction* ConfigFunction_new(const char* name) {
   ConfigFunction* function = calloc(1, sizeof(ConfigFunction));
   if (!function) {
      exit(EXIT_FAILURE);
   }
   function->name = xstrdup(name);
   function->subitems = Vector_new(4);
   function->details = Vector_new(8);
   return function;
}

static ConfigSubItem* ConfigSubItem_new(const char* name) {
   ConfigSubItem* subitem = calloc(1, sizeof(ConfigSubItem));
   if (!subitem) {
      exit(EXIT_FAILURE);
   }
   subitem->name = xstrdup(name);
   subitem->details = Vector_new(8);
   return subitem;
}

static void delete_detail_vector(Vector* details) {
   if (!details) {
      return;
   }
   for (size_t i = 0; i < Vector_size(details); i++) {
      free(Vector_get(details, i));
   }
   Vector_delete(details);
}

static void ConfigSubItem_delete(ConfigSubItem* subitem) {
   if (!subitem) {
      return;
   }
   free(subitem->name);
   delete_detail_vector(subitem->details);
   free(subitem);
}

static void ConfigFunction_delete(ConfigFunction* function) {
   if (!function) {
      return;
   }
   free(function->name);
   for (size_t i = 0; i < Vector_size(function->subitems); i++) {
      ConfigSubItem_delete(Vector_get(function->subitems, i));
   }
   Vector_delete(function->subitems);
   delete_detail_vector(function->details);
   free(function);
}

static void ConfigCategory_delete(ConfigCategory* category) {
   if (!category) {
      return;
   }
   free(category->name);
   for (size_t i = 0; i < Vector_size(category->functions); i++) {
      ConfigFunction_delete(Vector_get(category->functions, i));
   }
   Vector_delete(category->functions);
   free(category);
}

Config* Config_load(const char* path) {
   FILE* file = fopen(path, "r");
   if (!file) {
      return NULL;
   }

   Config* config = calloc(1, sizeof(Config));
   if (!config) {
      exit(EXIT_FAILURE);
   }
   config->categories = Vector_new(4);

   ConfigCategory* current_category = NULL;
   ConfigFunction* current_function = NULL;
   ConfigSubItem* current_subitem = NULL;

   char buffer[1024];
   while (fgets(buffer, sizeof(buffer), file)) {
      buffer[strcspn(buffer, "\r\n")] = '\0';
      const char* start = buffer;
      int indent = count_indent(buffer, &start);
      char* trimmed = trim_whitespace((char*)start);
      if (trimmed[0] == '\0') {
         continue;
      }
      if (is_separator_line(trimmed) || is_header_line(trimmed)) {
         continue;
      }

      if (indent == 0) {
         current_category = ConfigCategory_new(trimmed);
         Vector_add(config->categories, current_category);
         current_function = NULL;
         current_subitem = NULL;
         continue;
      }

      if (!current_category) {
         continue;
      }

      if (indent == 1) {
         current_function = ConfigFunction_new(trimmed);
         Vector_add(current_category->functions, current_function);
         current_subitem = NULL;
         continue;
      }

      if (!current_function) {
         continue;
      }

      if (indent == 2) {
         if (trimmed[0] == '[') {
            add_detail_line(current_function->details, trimmed, 0);
            current_subitem = NULL;
         } else {
            current_subitem = ConfigSubItem_new(trimmed);
            Vector_add(current_function->subitems, current_subitem);
         }
         continue;
      }

      if (indent >= 3) {
         int extra_indent = indent - (current_subitem ? 3 : 2);
         if (current_subitem) {
            add_detail_line(current_subitem->details, trimmed, extra_indent);
         } else {
            add_detail_line(current_function->details, trimmed, extra_indent);
         }
      }
   }

   fclose(file);
   return config;
}

void Config_delete(Config* config) {
   if (!config) {
      return;
   }
   for (size_t i = 0; i < Vector_size(config->categories); i++) {
      ConfigCategory_delete(Vector_get(config->categories, i));
   }
   Vector_delete(config->categories);
   free(config);
}

size_t Config_categoryCount(const Config* config) {
   return config ? Vector_size(config->categories) : 0;
}

const ConfigCategory* Config_getCategory(const Config* config, size_t index) {
   if (!config || index >= Vector_size(config->categories)) {
      return NULL;
   }
   return Vector_get(config->categories, index);
}

size_t Config_functionCount(const ConfigCategory* category) {
   return category ? Vector_size(category->functions) : 0;
}

const ConfigFunction* Config_getFunction(const ConfigCategory* category, size_t index) {
   if (!category || index >= Vector_size(category->functions)) {
      return NULL;
   }
   return Vector_get(category->functions, index);
}

size_t Config_subitemCount(const ConfigFunction* function) {
   return function ? Vector_size(function->subitems) : 0;
}

const ConfigSubItem* Config_getSubItem(const ConfigFunction* function, size_t index) {
   if (!function || index >= Vector_size(function->subitems)) {
      return NULL;
   }
   return Vector_get(function->subitems, index);
}

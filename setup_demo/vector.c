#include "vector.h"

#include <stdlib.h>

static void Vector_grow(Vector* vector, size_t min_capacity) {
   if (vector->capacity >= min_capacity) {
      return;
   }
   size_t new_capacity = vector->capacity ? vector->capacity * 2 : 8;
   if (new_capacity < min_capacity) {
      new_capacity = min_capacity;
   }
   void** items = realloc(vector->items, new_capacity * sizeof(void*));
   if (!items) {
      exit(EXIT_FAILURE);
   }
   vector->items = items;
   vector->capacity = new_capacity;
}

Vector* Vector_new(size_t capacity) {
   Vector* vector = calloc(1, sizeof(Vector));
   if (!vector) {
      exit(EXIT_FAILURE);
   }
   vector->capacity = capacity;
   if (capacity) {
      vector->items = calloc(capacity, sizeof(void*));
      if (!vector->items) {
         exit(EXIT_FAILURE);
      }
   }
   return vector;
}

void Vector_delete(Vector* vector) {
   if (!vector) {
      return;
   }
   free(vector->items);
   free(vector);
}

void Vector_clear(Vector* vector) {
   if (!vector) {
      return;
   }
   vector->size = 0;
}

void Vector_add(Vector* vector, void* item) {
   if (!vector) {
      return;
   }
   Vector_grow(vector, vector->size + 1);
   vector->items[vector->size++] = item;
}

void* Vector_get(const Vector* vector, size_t index) {
   if (!vector || index >= vector->size) {
      return NULL;
   }
   return vector->items[index];
}

size_t Vector_size(const Vector* vector) {
   return vector ? vector->size : 0;
}

#ifndef SETUP_DEMO_VECTOR_H
#define SETUP_DEMO_VECTOR_H

#include <stddef.h>

typedef struct Vector {
   void** items;
   size_t size;
   size_t capacity;
} Vector;

Vector* Vector_new(size_t capacity);
void Vector_delete(Vector* vector);
void Vector_clear(Vector* vector);
void Vector_add(Vector* vector, void* item);
void* Vector_get(const Vector* vector, size_t index);
size_t Vector_size(const Vector* vector);

#endif

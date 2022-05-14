#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "include/vector.h"

struct vector *vector_init() {
  struct vector *vector = calloc(1, sizeof *vector);
  if (!vector) return NULL;

  vector->data = calloc(INIT_CAPACITY, sizeof *vector->data);
  if (!vector->data) {
    free(vector);
    return NULL;
  }

  vector->capacity = INIT_CAPACITY;
  vector->size = 0;
  return vector;
}

void vector_destroy(struct vector *vector, void (*destroy)(void *element)) {
  if (!vector) return;
  if (vector->data) {
    for (unsigned long long i = 0; i < vector->size; i++) {
      if (destroy && vector->data[i]) {
        destroy(vector->data[i]);
      }
    }
    free(vector->data);
  }
  free(vector);
}

unsigned long long vector_size(struct vector *vector) { return vector->size; }

unsigned long long vector_capacity(struct vector *vector) {
  return vector->capacity;
}

bool vector_empty(struct vector *vector) {
  if (!vector) return true;
  return vector->size == 0;
}

void *vector_at(struct vector *vector, unsigned long long pos) {
  if (!vector) return NULL;
  if (!vector->data) return NULL;
  if (pos >= vector->size) return NULL;

  return vector->data[pos];
}

void *vector_find(struct vector *vector, void *element,
                  int (*cmpr)(const void *, const void *)) {
  if (!vector) return NULL;
  if (!vector->data) return NULL;
  if (!cmpr) return NULL;

  vector_sort(vector, cmpr);
  void *elem =
      bsearch(&element, vector->data, vector->size, sizeof *vector->data, cmpr);
  if (!elem) return NULL;
  return *(void **)elem;
}

/* used internally to resize the vector by GROWTH_FACTOR */
bool vector_resize(struct vector *vector) {
  // limit check. vector:capacity cannot exceed LLONG_MAX
  if (LLONG_MAX >> GROWTH_FACTOR < vector->capacity) return false;
  unsigned long long new_capacity = vector->capacity << GROWTH_FACTOR;

  void **tmp = realloc(vector->data, new_capacity * sizeof *vector->data);
  if (!tmp) return false;

  vector->capacity = new_capacity;
  vector->data = tmp;
  return true;
}

unsigned long long vector_reserve(struct vector *vector,
                                  unsigned long long size) {
  if (!vector) return 0;
  if (size > LLONG_MAX) return vector->capacity;
  if (size <= vector->capacity) return vector->capacity;

  void **tmp = realloc(vector->data, size * sizeof *vector->data);
  if (!tmp) return vector->capacity;

  vector->capacity = size;
  vector->data = tmp;
  return vector->capacity;
}

bool vector_push(struct vector *vector, void *element) {
  if (!vector) return false;
  if (!vector->data) return false;
  if (vector->size == vector->capacity) {
    if (!vector_resize(vector)) return false;
  }

  vector->data[vector->size] = element;
  vector->size++;
  return true;
}

void *vector_pop(struct vector *vector) {
  if (!vector) return NULL;
  if (!vector->data) return NULL;
  return vector->data[--vector->size];
}

void *vector_remove_at(struct vector *vector, unsigned long long pos) {
  void *old = vector_at(vector, pos);
  if (!old) return NULL;

  memmove(vector->data + pos, vector->data + pos + 1, vector->size - pos - 1);
  vector->size--;
  return old;
}

void *vector_replace(struct vector *vector, void *element,
                     unsigned long long pos) {
  void *old = vector_at(vector, pos);
  if (!old) return NULL;

  vector->data[pos] = element;
  return old;
}

unsigned long long vector_shrink(struct vector *vector) {
  if (!vector) return 0;
  if (!vector->data) return 0;

  unsigned long long new_capacity = vector->size;
  void **tmp = realloc(vector->data, new_capacity * sizeof *vector->data);
  if (!tmp) return vector->capacity;

  vector->capacity = new_capacity;
  vector->data = tmp;
  return vector->capacity;
}

long long vector_index_of(struct vector *vector, void *element,
                          bool (*equals)(const void *, const void *)) {
  if (!vector) return -1;
  if (!vector->data) return -1;

  for (unsigned long long i = 0; i < vector->size; i++) {
    if (equals(element, vector->data[i])) return i;
  }

  return -1;
}

void vector_sort(struct vector *vector,
                 int (*cmpr)(const void *, const void *)) {
  if (!vector) return;
  if (!vector->data) return;

  qsort(vector->data, vector->size, sizeof *vector->data, cmpr);
}
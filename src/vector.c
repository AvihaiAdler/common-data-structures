#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "include/vector.h"

struct vector *vector_init(unsigned long long data_size) {
  struct vector *vector = calloc(1, sizeof *vector);
  if (!vector) return NULL;

  vector->data_size = data_size;
  vector->data = calloc(INIT_CAPACITY * vector->data_size, 1);
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
    for (unsigned long long i = 0; i < vector->size * vector->data_size;
         i += vector->data_size) {
      if (destroy) {
        destroy(&vector->data[i]);
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

  return &vector->data[pos * vector->data_size];
}

void *vector_find(struct vector *vector, void *element,
                  int (*cmpr)(const void *, const void *)) {
  if (!vector) return NULL;
  if (!vector->data) return NULL;
  if (!cmpr) return NULL;

  vector_sort(vector, cmpr);
  void *elem =
      bsearch(element, vector->data, vector->size, vector->data_size, cmpr);
  if (!elem) return NULL;
  return elem;
}

/* used internally to resize the vector by GROWTH_FACTOR */
static bool vector_resize(struct vector *vector) {
  // limit check. vector:capacity cannot exceeds LLONG_MAX
  if (LLONG_MAX >> GROWTH_FACTOR < vector->capacity) return false;
  unsigned long long new_capacity = vector->capacity << GROWTH_FACTOR;

  // limit check. vector::capacity * vector::data_size (the max number of
  // element the vector can hold) cannot exceeds LLONG_MAX / vector::data_size
  // (the number of elements LLONG_MAX can hold)
  if (LLONG_MAX / vector->data_size < new_capacity * vector->data_size)
    return false;

  unsigned char *tmp = realloc(vector->data, new_capacity * vector->data_size);
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

  unsigned char *tmp = realloc(vector->data, size * vector->data_size);
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

  memcpy(&vector->data[vector->size * vector->data_size], element,
         vector->data_size);
  vector->size++;
  return true;
}

void *vector_pop(struct vector *vector) {
  if (!vector) return NULL;
  if (!vector->data) return NULL;
  return &vector->data[--vector->size * vector->data_size];
}

void *vector_remove_at(struct vector *vector, unsigned long long pos) {
  void *tmp = vector_at(vector, pos);
  if (!tmp) return NULL;

  unsigned char *old = calloc(1, vector->data_size);
  if (!old) return NULL;

  memcpy(old, tmp, vector->data_size);

  unsigned long long factored_pos = pos * vector->data_size;
  memmove(vector->data + factored_pos, vector->data + factored_pos + 1,
          vector->size * vector->data_size - factored_pos - vector->data_size);
  vector->size--;
  return old;
}

void *vector_replace(struct vector *vector, void *element,
                     unsigned long long pos) {
  void *tmp = vector_at(vector, pos);
  if (!tmp) return NULL;

  unsigned char *old = calloc(vector->data_size, 1);
  if (!old) return NULL;

  memcpy(old, tmp, vector->data_size);

  memcpy(&vector->data[pos * vector->data_size], element, vector->data_size);
  return old;
}

unsigned long long vector_shrink(struct vector *vector) {
  if (!vector) return 0;
  if (!vector->data) return 0;

  unsigned long long new_capacity = vector->size;
  unsigned char *tmp = realloc(vector->data, new_capacity * vector->data_size);
  if (!tmp) return vector->capacity;

  vector->capacity = new_capacity;
  vector->data = tmp;
  return vector->capacity;
}

long long vector_index_of(struct vector *vector, void *element,
                          bool (*equals)(const void *, const void *)) {
  if (!vector) return -1;
  if (!vector->data) return -1;

  for (unsigned long long i = 0; i < vector->size * vector->data_size;
       i += vector->data_size) {
    if (equals(element, &vector->data[i])) return i / vector->data_size;
  }

  return -1;
}

void vector_sort(struct vector *vector,
                 int (*cmpr)(const void *, const void *)) {
  if (!vector) return;
  if (!vector->data) return;

  qsort(vector->data, vector->size, vector->data_size, cmpr);
}
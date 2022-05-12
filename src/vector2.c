#include "include/vector2.h"

#include <string.h>

struct list *list_init() {
  struct list *list = calloc(1, sizeof *list);
  if (!list) return NULL;

  list->data = calloc(INIT_CAPACITY, sizeof *list->data);
  if (!list->data) {
    free(list);
    return NULL;
  }

  list->capacity = INIT_CAPACITY;
  list->size = 0;
  return list;
}

void list_destroy(struct list *list, void (*destroy)(void *element)) {
  if (!list) return;
  if (list->data) {
    for (size_t i = 0; i < list->size; i++) {
      if (destroy && list->data[i]) {
        destroy(list->data[i]);
      }
    }
    free(list->data);
  }
  free(list);
}

bool list_empty(struct list *list) {
  if (!list) return false;
  return list->size == 0;
}

void *list_at(struct list *list, size_t pos) {
  if (!list) return NULL;
  if (!list->data) return NULL;
  if (pos >= list->size) return NULL;

  return list->data[pos];
}

void *list_find(struct list *list, void *element,
                int (*cmpr)(const void *, const void *)) {
  if (!list) return NULL;
  if (!list->data) return NULL;
  if (!cmpr) return NULL;

  list_sort(list, cmpr);
  return bsearch(element, list->data, list->size, sizeof *list->data, cmpr);
}

bool list_resize(struct list *list) {
  if (SIZE_MAX >> GROWTH_FACTOR >= list->capacity) return false;
  size_t new_capacity = list->capacity << GROWTH_FACTOR;

  void **tmp = realloc(list->data, new_capacity);
  if (!tmp) return false;

  list->capacity = new_capacity;
  list->data = tmp;
  return true;
}

size_t list_reserve(struct list *list, size_t size) {
  if (!list) return 0;
  if (size <= list->capacity) return list->capacity;

  void **tmp = realloc(list->data, size);
  if (!tmp) return list->capacity;

  list->capacity = size;
  list->data = tmp;
}

bool list_push(struct list *list, void *element) {
  if (!list) return false;
  if (!list->data) return false;
  if (list->size == list->capacity) {
    if (!list_resize(list)) return false;
  }

  list->data[list->size] = element;
  list->size++;
  return true;
}

void *list_pop(struct list *list) {
  if (!list) return NULL;
  if (!list->data) return NULL;
  return list->data[--list->size];
}

void *list_remove_at(struct list *list, size_t pos) {
  void *old = list_at(list, pos);
  if (!old) return NULL;

  memmove(list->data + pos, list->data + pos + 1, list->size - pos - 1);
  list->size--;
  return old;
}

void *list_replace(struct list *list, void *element, size_t pos) {
  void *old = list_at(list, pos);
  if (!old) return NULL;

  list->data[pos] = element;
  return old;
}

size_t list_shrink(struct list *list) {
  if (!list) return 0;
  if (!list->data) return 0;

  size_t new_capacity = list->size + 1;
  void **tmp = realloc(list->data, new_capacity);
  if (!tmp) return list->capacity;

  list->capacity = new_capacity;
  list->data = tmp;
  return list->capacity;
}

int64_t list_index_of(struct list *list, void *element,
                      bool (*equals)(const void *, const void *)) {
  if (!list) return -1;
  if (!list->data) return -1;

  for (size_t i = 0; i < list->size; i++) {
    if (equals(element, list->data[i])) return i;
  }

  return -1;
}

void list_sort(struct list *list, int (*cmpr)(const void *, const void *)) {
  if (!list) return;
  if (!list->data) return;

  qsort(list->data, list->size, sizeof *list->data, cmpr);
}
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "include/list.h"

struct point {
  int x, y;
};

bool equals(const void *a, const void *b) {
  const struct point *p_a = a;
  const struct point *p_b = b;
  return p_a->x == p_b->x && p_a->y == p_b->y;
}

struct list *before(struct point *points, size_t arr_size) {
  struct list *list = list_init();
  for (size_t i = 0; i < arr_size; i++) {
    list_prepend(list, &points[i]);
  }
  return list;
}

void destroy(struct point *point) {
  if (point) free(point);
}

void after(struct list *list) { list_destroy(list, destroy); }

void list_prepend_test(struct point **points, size_t arr_size) {
  // given
  struct list *list = list_init();

  assert(list);
  assert(list_empty(list));
  assert(list_size(list) == 0);
  assert(!list->head && !list->tail);

  // when
  for (size_t i = 0; i < arr_size; i++) {
    list_prepend(list, points[i]);
  }

  // then
  assert(list->size == arr_size);
  assert(equals(list_peek_first(list), points[arr_size - 1]));
  assert(equals(list_peek_last(list), points[0]));

  after(list);
}

void list_append_test(struct point **points, size_t arr_size) {
  // given
  struct list *list = list_init();

  assert(list);
  assert(list_empty(list));
  assert(list_size(list) == 0);
  assert(!list->tail && !list->head);

  // when
  for (size_t i = 0; i < arr_size; i++) {
    list_append(list, points[i]);
  }

  // then
  assert(list->size == arr_size);
  assert(equals(list_peek_first(list), points[0]));
  assert(equals(list_peek_last(list), points[arr_size - 1]));

  after(list);
}

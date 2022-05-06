#include "int_vector_tests.h"

#include <assert.h>

#include "vector.h"

GENERATE_VECT(int, int)
#define NUM_OF_ELEMENTS 15

bool equals(int a, int b) { return a == b; }

int int_comparator(const void *a, const void *b) {
  const struct int_wrapper *w_a = a;
  const struct int_wrapper *w_b = b;
  return (w_a->value > w_b->value) - (w_a->value < w_b->value);
}

static struct int_vector *before(void) {
  struct int_vector *int_vect = int_vect_init();
  for (int i = NUM_OF_ELEMENTS; i > 0; i--) {
    int_vect_push(int_vect, i);
  }
  return int_vect;
}

static void after(struct int_vector *int_vect) {
  int_vect_destroy(int_vect, NULL);
}

void int_vect_push_test() {
  // given
  // when
  struct int_vector *int_vect = before();

  // then
  assert(int_vect->size == NUM_OF_ELEMENTS);

  after(int_vect);
}

void int_vect_pop_test() {
  // given
  struct int_vector *int_vect = before();

  struct int_wrapper *tmp = int_vect_at(int_vect, int_vect->size - 1);
  assert(tmp);
  assert(tmp->value == 1);

  // when
  tmp = int_vect_pop(int_vect);
  assert(tmp);
  free(tmp);

  // then
  assert(int_vect->size == NUM_OF_ELEMENTS - 1);
  tmp = int_vect_at(int_vect, int_vect->size - 1);
  assert(tmp);
  assert(tmp->value == 2);

  after(int_vect);
}

void int_vect_at_test() {
  // given
  struct int_vector *int_vect = before();

  // when
  struct int_wrapper *first = int_vect_at(int_vect, 0);
  struct int_wrapper *last = int_vect_at(int_vect, NUM_OF_ELEMENTS - 1);

  // then
  assert(first);
  assert(last);
  assert(first->value == NUM_OF_ELEMENTS);
  assert(last->value == 1);

  after(int_vect);
}

void int_vect_replace_test() {
  // given
  struct int_vector *int_vect = before();

  // when
  struct int_wrapper *old_val = int_vect_replace(int_vect, 3, 100);
  free(old_val);

  // then
  struct int_wrapper *new_val = int_vect_at(int_vect, 3);
  assert(new_val);
  assert(new_val->value == 100);

  after(int_vect);
}

void int_vect_index_of_test() {
  // given
  struct int_vector *int_vect = before();

  // when
  int64_t val = int_vect_index_of(int_vect, NUM_OF_ELEMENTS - 1, equals);

  // then
  assert(val == 1);

  after(int_vect);
}

void int_vect_sort_test() {
  // given
  struct int_vector *int_vect = before();

  struct int_wrapper *first = int_vect_at(int_vect, 0);
  struct int_wrapper *last = int_vect_at(int_vect, NUM_OF_ELEMENTS - 1);
  assert(first);
  assert(last);
  assert(first->value == NUM_OF_ELEMENTS);
  assert(last->value == 1);

  // when
  int_vect_sort(int_vect, int_comparator);

  // then
  first = int_vect_at(int_vect, 0);
  last = int_vect_at(int_vect, NUM_OF_ELEMENTS - 1);
  assert(first);
  assert(last);
  assert(first->value == 1);
  assert(last->value == NUM_OF_ELEMENTS);

  after(int_vect);
}
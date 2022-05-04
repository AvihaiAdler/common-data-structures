#include "int_vector_tests.h"

#include <assert.h>

#include "vector.h"

GENERATE_VECT(int, int)
#define NUM_OF_ELEMENTS 15

bool equals(int a, int b) { return a == b; }

int int_comparator(const void *a, const void *b) {
  int i_a = *(int *)a;
  int i_b = *(int *)b;
  return (i_a > i_b) - (i_a < i_b);
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

  assert(int_vect_at(int_vect, int_vect->size - 1) == 1);

  // when
  int returned = int_vect_pop(int_vect);

  // then
  assert(int_vect->size == NUM_OF_ELEMENTS - 1);
  assert(returned == 1);
  assert(int_vect_at(int_vect, int_vect->size - 1) == returned + 1);

  after(int_vect);
}

void int_vect_at_test() {
  // given
  struct int_vector *int_vect = before();

  // when
  int first = int_vect_at(int_vect, 0);
  int last = int_vect_at(int_vect, NUM_OF_ELEMENTS - 1);

  // then
  assert(first == NUM_OF_ELEMENTS);
  assert(last == 1);

  after(int_vect);
}

void int_vect_replace_test() {
  // given
  struct int_vector *int_vect = before();

  // when
  int_vect_replace(int_vect, 3, 100);

  // then
  int new_val = int_vect_at(int_vect, 3);
  assert(new_val == 100);

  after(int_vect);
}

void int_vect_index_of_test() {
  // given
  struct int_vector *int_vect = before();

  // when
  long val = int_vect_index_of(int_vect, NUM_OF_ELEMENTS - 1, equals);

  // then
  assert(val == 1);

  after(int_vect);
}

void int_vect_sort_test() {
  // given
  struct int_vector *int_vect = before();

  int first = int_vect_at(int_vect, 0);
  int last = int_vect_at(int_vect, NUM_OF_ELEMENTS - 1);
  assert(first == NUM_OF_ELEMENTS);
  assert(last == 1);

  // when
  int_vect_sort(int_vect, int_comparator);

  // then
  first = int_vect_at(int_vect, 0);
  last = int_vect_at(int_vect, NUM_OF_ELEMENTS - 1);
  assert(first == 1);
  assert(last == NUM_OF_ELEMENTS);

  after(int_vect);
}
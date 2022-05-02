#include "int_vector_tests.h"

#include <assert.h>
#include <include/vector.h>

GENERATE_VECT(int, int)
#define NUM_OF_ELEMENTS 15

int int_comparator(const void *a, const void *b) {
  int i_a = *(int *)a;
  int i_b = *(int *)b;
  return (i_a > i_b) - (i_a < i_b);
}

struct Vector_int *before(void) {
  struct Vector_int *vect_int = vect_int_init();
  for (int i = NUM_OF_ELEMENTS; i > 0; i--) {
    vect_int_push(vect_int, i);
  }
  return vect_int;
}

void after(struct Vector_int *vect_int) { vect_destroy(vect_int); }

void vect_int_push_test() {
  // given
  // when
  struct Vector_int *vect_int = before();

  // then
  assert(vect_int->size == NUM_OF_ELEMENTS);

  after(vect_int);
}

void vect_int_pop_test() {
  // given
  struct Vector_int *vect_int = before();

  assert(vect_int_at(vect_int, vect_int->size - 1) == 1);

  // when
  int returned = vect_int_pop(vect_int);

  // then
  assert(vect_int->size == NUM_OF_ELEMENTS - 1);
  assert(returned == 1);
  assert(vect_int_at(vect_int, vect_int->size - 1) == returned + 1);

  after(vect_int);
}

void vect_int_at_test() {
  // given
  struct Vector_int *vect_int = before();

  // when
  int first = vect_int_at(vect_int, 0);
  int last = vect_int_at(vect_int, NUM_OF_ELEMENTS - 1);

  // then
  assert(first == NUM_OF_ELEMENTS);
  assert(last == 1);

  after(vect_int);
}

void vect_int_replace_test() {
  // given
  struct Vector_int *vect_int = before();

  // when
  vect_int_replace(vect_int, 3, 100);

  // then
  int new_val = vect_int_at(vect_int, 3);
  assert(new_val == 100);

  after(vect_int);
}

void vect_int_index_of_test() {
  // given
  struct Vector_int *vect_int = before();

  // when
  long val = vect_int_index_of(vect_int, NUM_OF_ELEMENTS - 1, int_comparator);

  // then
  assert(val == 1);

  after(vect_int);
}

void vect_int_sort_test() {
  // given
  struct Vector_int *vect_int = before();

  int first = vect_int_at(vect_int, 0);
  int last = vect_int_at(vect_int, NUM_OF_ELEMENTS - 1);
  assert(first == NUM_OF_ELEMENTS);
  assert(last == 1);

  // when
  vect_int_sort(vect_int, int_comparator);

  // then
  first = vect_int_at(vect_int, 0);
  last = vect_int_at(vect_int, NUM_OF_ELEMENTS - 1);
  assert(first == 1);
  assert(last == NUM_OF_ELEMENTS);

  after(vect_int);
}
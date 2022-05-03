#include "str_vector_tests.h"

#include <assert.h>
#include <include/vector.h>
#include <string.h>

#define NUM_OF_ELEMENTS 4
GENERATE_VECT(const char *, str)

int str_comparator(const void *a, const void *b) {
  const char *s_a = *(const char **)a;
  const void *s_b = *(const char **)b;
  return strcmp(s_a, s_b);
}

static struct vector_str *before(void) {
  struct vector_str *vect_str = vect_str_init();

  vect_str_push(vect_str, "simple");
  vect_str_push(vect_str, "example");
  vect_str_push(vect_str, "using");
  vect_str_push(vect_str, "strings");

  return vect_str;
}

static void after(struct vector_str *vect_str) { vect_destroy(vect_str); }

void vect_str_push_test() {
  // given
  // when
  struct vector_str *vect_str = before();

  // then
  assert(vect_str->size == NUM_OF_ELEMENTS);

  after(vect_str);
}

void vect_str_pop_test() {
  // given
  struct vector_str *vect_str = before();

  assert(strcmp(vect_str_at(vect_str, vect_str->size - 1), "strings") == 0);

  // when
  const char *returned = vect_str_pop(vect_str);

  // then
  assert(vect_str->size == NUM_OF_ELEMENTS - 1);
  assert(strcmp(returned, "strings") == 0);
  assert(strcmp(vect_str_at(vect_str, vect_str->size - 1), "using") == 0);

  after(vect_str);
}

void vect_str_at_test() {
  // given
  struct vector_str *vect_str = before();

  // when
  const char *first = vect_str_at(vect_str, 0);
  const char *last = vect_str_at(vect_str, NUM_OF_ELEMENTS - 1);

  // then
  assert(strcmp(first, "simple") == 0);
  assert(strcmp(last, "strings") == 0);

  after(vect_str);
}

void vect_str_replace_test() {
  // given
  struct vector_str *vect_str = before();

  // when
  vect_str_replace(vect_str, 3, "another");

  // then
  const char *new_val = vect_str_at(vect_str, 3);
  assert(strcmp(new_val, "another") == 0);

  after(vect_str);
}

void vect_str_index_of_test() {
  // given
  struct vector_str *vect_str = before();

  // when
  long index = vect_str_index_of(vect_str, "using", str_comparator);

  // then
  assert(index == 2);

  after(vect_str);
}

void vect_str_sort_test() {
  // given
  struct vector_str *vect_str = before();

  const char *first = vect_str_at(vect_str, 0);
  const char *last = vect_str_at(vect_str, NUM_OF_ELEMENTS - 1);

  // when
  vect_str_sort(vect_str, str_comparator);

  // then
  first = vect_str_at(vect_str, 0);
  last = vect_str_at(vect_str, NUM_OF_ELEMENTS - 1);
  assert(strcmp(first, "example") == 0);
  assert(strcmp(last, "using") == 0);

  after(vect_str);
}
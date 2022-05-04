#include "str_vector_tests.h"

#include <assert.h>
#include <string.h>

#include "vector.h"

#define NUM_OF_ELEMENTS 4
GENERATE_VECT(const char *, str)

static bool equals(const char *a, const char *b) { return strcmp(a, b) == 0; }

int str_comparator(const void *a, const void *b) {
  const char *s_a = *(const char **)a;
  const void *s_b = *(const char **)b;
  return strcmp(s_a, s_b);
}

static struct str_vector *before(void) {
  struct str_vector *str_vect = str_vect_init();

  str_vect_push(str_vect, "simple");
  str_vect_push(str_vect, "example");
  str_vect_push(str_vect, "using");
  str_vect_push(str_vect, "strings");

  return str_vect;
}

static void after(struct str_vector *str_vect) {
  str_vect_destroy(str_vect, NULL);
}

void str_vect_push_test() {
  // given
  // when
  struct str_vector *str_vect = before();

  // then
  assert(str_vect->size == NUM_OF_ELEMENTS);

  after(str_vect);
}

void str_vect_pop_test() {
  // given
  struct str_vector *str_vect = before();

  assert(strcmp(str_vect_at(str_vect, str_vect->size - 1), "strings") == 0);

  // when
  const char *returned = str_vect_pop(str_vect);

  // then
  assert(str_vect->size == NUM_OF_ELEMENTS - 1);
  assert(strcmp(returned, "strings") == 0);
  assert(strcmp(str_vect_at(str_vect, str_vect->size - 1), "using") == 0);

  after(str_vect);
}

void str_vect_at_test() {
  // given
  struct str_vector *str_vect = before();

  // when
  const char *first = str_vect_at(str_vect, 0);
  const char *last = str_vect_at(str_vect, NUM_OF_ELEMENTS - 1);

  // then
  assert(strcmp(first, "simple") == 0);
  assert(strcmp(last, "strings") == 0);

  after(str_vect);
}

void str_vect_replace_test() {
  // given
  struct str_vector *str_vect = before();

  // when
  str_vect_replace(str_vect, 3, "another");

  // then
  const char *new_val = str_vect_at(str_vect, 3);
  assert(strcmp(new_val, "another") == 0);

  after(str_vect);
}

void str_vect_index_of_test() {
  // given
  struct str_vector *str_vect = before();

  // when
  int64_t index = str_vect_index_of(str_vect, "using", equals);

  // then
  assert(index == 2);

  after(str_vect);
}

void str_vect_sort_test() {
  // given
  struct str_vector *str_vect = before();

  const char *first = str_vect_at(str_vect, 0);
  const char *last = str_vect_at(str_vect, NUM_OF_ELEMENTS - 1);

  // when
  str_vect_sort(str_vect, str_comparator);

  // then
  first = str_vect_at(str_vect, 0);
  last = str_vect_at(str_vect, NUM_OF_ELEMENTS - 1);
  assert(strcmp(first, "example") == 0);
  assert(strcmp(last, "using") == 0);

  after(str_vect);
}
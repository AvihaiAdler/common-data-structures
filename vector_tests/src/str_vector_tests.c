#include "str_vector_tests.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

GENERATE_VECT(const char *, str)

static const char *arr[] = {"simple", "example", "using", "strings"};

static bool equals(const char *a, const char *b) { return strcmp(a, b) == 0; }

int str_comparator(const void *a, const void *b) {
  const char *s_a = *(const char **)a;
  const void *s_b = *(const char **)b;
  return strcmp(s_a, s_b);
}

static struct str_vector *before(void) {
  struct str_vector *str_vect = str_vect_init();

  size_t size = sizeof arr / sizeof *arr;
  for (size_t i = 0; i < size; i++) {
    str_vect_push(str_vect, arr[i]);
  }

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
  size_t size = sizeof arr / sizeof *arr;
  assert(str_vect->size == size);

  after(str_vect);
}

void str_vect_pop_test() {
  // given
  struct str_vector *str_vect = before();

  size_t size = sizeof arr / sizeof *arr;
  struct str_wrapper *at = str_vect_at(str_vect, str_vect->size - 1);
  assert(strcmp(at->value, arr[size - 1]) == 0);

  // when
  struct str_wrapper *returned = str_vect_pop(str_vect);

  // then
  assert(str_vect->size == size - 1);
  assert(strcmp(returned->value, arr[size - 1]) == 0);

  at = str_vect_at(str_vect, str_vect->size - 1);
  assert(strcmp(at->value, arr[size - 2]) == 0);

  free(returned);
  after(str_vect);
}

void str_vect_at_test() {
  // given
  struct str_vector *str_vect = before();

  // when
  size_t size = sizeof arr / sizeof *arr;
  struct str_wrapper *first = str_vect_at(str_vect, 0);
  struct str_wrapper *last = str_vect_at(str_vect, size - 1);

  // then
  assert(strcmp(first->value, arr[0]) == 0);
  assert(strcmp(last->value, arr[size - 1]) == 0);

  after(str_vect);
}

void str_vect_replace_test() {
  // given
  struct str_vector *str_vect = before();

  // when
  const char *replacement = "another";
  unsigned int index = 3;
  struct str_wrapper *replaced = str_vect_replace(str_vect, index, replacement);

  // then
  struct str_wrapper *new_val = str_vect_at(str_vect, index);
  assert(strcmp(new_val->value, replacement) == 0);
  assert(strcmp(replaced->value, arr[index]) == 0);

  free(replaced);
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

  size_t size = sizeof arr / sizeof *arr;
  struct str_wrapper *first = str_vect_at(str_vect, 0);
  struct str_wrapper *last = str_vect_at(str_vect, size - 1);

  // when
  str_vect_sort(str_vect, str_comparator);

  // then
  first = str_vect_at(str_vect, 0);
  last = str_vect_at(str_vect, size - 1);
  assert(strcmp(first->value, "example") == 0);
  assert(strcmp(last->value, "using") == 0);

  after(str_vect);
}
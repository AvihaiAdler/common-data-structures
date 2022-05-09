#include <stdio.h>
#include <string.h>

#include "int_vector_tests.h"
#include "str_vector_tests.h"
#include "word_vector_tests.h"

void int_vect_unit_tests(void) {
  puts("starting int_vector unit tests...");

  int_vect_push_test();
  int_vect_pop_test();
  int_vect_at_test();
  int_vect_replace_test();
  int_vect_index_of_test();
  int_vect_sort_test();

  puts("int_vector tests passed successfully");
}

void str_vect_unit_tests(void) {
  puts("starting str_vector (const char *) unit tests...");

  str_vect_push_test();
  str_vect_pop_test();
  str_vect_at_test();
  str_vect_replace_test();
  str_vect_index_of_test();
  str_vect_sort_test();

  puts("str_vector tests passed successfully");
}

void word_vect_unit_test_with_heap_allocation(void) {
  puts("starting word_vector unit test...");
  word_vect_push_test();
  puts("word_vector tests passed successfully");
}

int main(void) {
  int_vect_unit_tests();

  str_vect_unit_tests();

  word_vect_unit_test_with_heap_allocation();
  return 0;
}
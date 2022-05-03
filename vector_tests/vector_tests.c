#include <stdio.h>
#include <string.h>

#include "int_vector_tests.h"
#include "str_vector_tests.h"

void int_vect_unit_tests(void) {
  puts("starting int_vector unit tests...");

  vect_int_push_test();
  vect_int_pop_test();
  vect_int_at_test();
  vect_int_replace_test();
  vect_int_index_of_test();
  vect_int_sort_test();

  puts("int_vector tests passed successfully");
}

void str_vect_unit_tests(void) {
  puts("starting str_vector (const char *) unit tests...");

  vect_str_push_test();
  vect_str_pop_test();
  vect_str_at_test();
  vect_str_replace_test();
  vect_str_index_of_test();
  vect_str_sort_test();

  puts("str_vector tests passed successfully");
}

int main(void) {
  int_vect_unit_tests();

  str_vect_unit_tests();
  return 0;
}
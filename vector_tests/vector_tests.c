#include <stdio.h>

#include "int_vector_tests.h"

int main(void) {
  vect_int_push_test();

  vect_int_pop_test();

  vect_int_at_test();

  vect_int_replace_test();

  vect_int_index_of_test();

  vect_int_sort_test();
  return 0;
}
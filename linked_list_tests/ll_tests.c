#include <stdio.h>

#include "point_ll_tests.h"

int main(void) {
  puts("starting point_ll unit tests...");
  point_ll_append_test();
  point_ll_prepend_test();
  point_ll_insert_higher_test();
  puts("point_ll tests passed successfully");
  return 0;
}
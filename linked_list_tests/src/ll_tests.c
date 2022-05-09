#include <stdio.h>

#include "point_ll_tests.h"

int main(void) {
  puts("starting point_ll unit tests...");
  point_ll_append_test();
  point_ll_prepend_test();
  point_ll_insert_higher_test();
  point_ll_remove_first_test();
  point_ll_remove_last_test();
  point_ll_replace_head_test();
  point_ll_replace_tail_test();
  point_ll_replace_test();
  puts("point_ll tests passed successfully");
  return 0;
}
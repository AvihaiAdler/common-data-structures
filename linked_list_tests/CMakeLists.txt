add_executable(
  ll_tests
  src/ll_tests.c
  src/point_ll_tests.c
)
target_include_directories(ll_tests PRIVATE ../include ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(ll_tests PUBLIC -Wall -Wextra -pedantic -fsanitize=address,undefined)
target_link_options(ll_tests PUBLIC -fsanitize=address,undefined)
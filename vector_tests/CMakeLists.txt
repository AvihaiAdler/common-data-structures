# include_directories(../include)
add_executable(
  vector_tests 
  src/vector_tests.c 
  src/int_vector_tests.c
  src/str_vector_tests.c
  src/word_vector_tests.c
)             # create an executable named vector_tests from vector_tests.c
target_include_directories(vector_tests PRIVATE ../include ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(vector_tests PUBLIC -Wall -Wextra -pedantic -fsanitize=address,undefined)
target_link_options(vector_tests PUBLIC -fsanitize=address,undefined)
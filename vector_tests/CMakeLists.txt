add_executable(
  vector_tests 
  vector_tests.c 
  int_vector_tests.c
)             # create an executable named vector_tests from vector_tests.c

target_link_libraries(vector_tests PRIVATE vector_lib)  # have the executable link against some library 
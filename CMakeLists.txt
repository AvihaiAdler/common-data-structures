cmake_minimum_required(VERSION 3.20)
project(generic_ds VERSION 0.0.1)

add_library(generics)

target_sources(generics 
  PRIVATE src/vec.c
  PRIVATE src/list.c
  PRIVATE src/hash_table.c
  PRIVATE src/bst.c
  PRIVATE src/ascii_str.c
)

target_include_directories(generics 
                          PRIVATE .
                          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(generics PRIVATE c_std_99)
target_compile_options(generics PRIVATE -Wall -Wextra -pedantic -O3)

include(CTest)
add_subdirectory(tests/vector)
add_subdirectory(tests/linked_list)
add_subdirectory(tests/hash_table)
add_subdirectory(tests/bst)
add_subdirectory(tests/ascii_str)

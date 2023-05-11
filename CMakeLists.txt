cmake_minimum_required(VERSION 3.20)
project(generic_ds VERSION 0.0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# add_subdirectory(dev)
include(CTest)
add_subdirectory(tests/vector)
add_subdirectory(tests/linked_list)
add_subdirectory(tests/hash_table)
add_subdirectory(tests/bst)
add_subdirectory(tests/ascii_str)

add_library(
  generics
  src/vec.c
  src/list.c
  src/hash_table.c
  src/bst.c
  src/ascii_str.c
)
set_target_properties(
  generics PROPERTIES 
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER include/vec.h 
  PUBLIC_HEADER include/list.h
  PUBLIC_HEADER include/hash_table.h
  PUBLIC_HEADER include/bst.h
  PUBLIC_HEADER include/ascii_str.h
)
target_include_directories(generics 
                          PRIVATE .
                          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

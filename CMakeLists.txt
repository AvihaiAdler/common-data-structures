cmake_minimum_required(VERSION 3.20)
project(generic_ds VERSION 0.0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# add_subdirectory(dev)
include(CTest)
enable_testing()
add_subdirectory(tests/vector)
add_subdirectory(tests/linked_list)

add_library(
  generics
  src/vector.c
  src/list.c
)
set_target_properties(
  generics PROPERTIES 
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER include/vector.h 
  PUBLIC_HEADER include/list.h
)
target_include_directories(generics PRIVATE .)
# add_compile_options(-Wall -Wextra -pedantic -fsanitize=address,undefined)
# add_link_options(-fsanitize=address,undefined)
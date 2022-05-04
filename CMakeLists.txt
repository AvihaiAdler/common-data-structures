cmake_minimum_required(VERSION 3.20)
project(generic_ds VERSION 0.0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
add_subdirectory(vector_tests)    # the rest of the directories
add_compile_options(-Wall -Wextra -pedantic)
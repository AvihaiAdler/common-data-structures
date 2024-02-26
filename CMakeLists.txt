cmake_minimum_required(VERSION 3.20)
project(ds VERSION 0.4.0)

add_library(ds)

target_sources(ds PRIVATE
  src/vec.c
  src/list.c
  src/hash_table.c
  src/bst.c
  src/ascii_str.c
  src/pair.c
  src/queue.c
)

target_compile_features(ds
  PRIVATE c_std_99
)

target_compile_options(ds
  PRIVATE
  $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -Wall -Wextra -Wpedantic -g>
  $<$<C_COMPILER_ID:MSVC>: -W4>
)

target_include_directories(ds
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_library(math
  NAMES m
  REQUIRED
)

target_link_libraries(ds
  PRIVATE ${math}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(CTest)
  add_subdirectory(tests)

  target_compile_options(ds
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -Og -fsanitize=address,undefined>
  )

  target_link_options(ds
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -fsanitize=address,undefined>
  )
else()
  target_compile_options(ds
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -O3>
  )
endif()

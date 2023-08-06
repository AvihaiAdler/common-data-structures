cmake_minimum_required(VERSION 3.20)
project(ds VERSION 0.1.0)

add_library(ds)

target_sources(ds
  PRIVATE src/vec.c
  PRIVATE src/list.c
  PRIVATE src/hash_table.c
  PRIVATE src/bst.c
  PRIVATE src/ascii_str.c
)

target_include_directories(ds
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ds
  PRIVATE c_std_99
)

target_compile_options(ds
  PRIVATE
  $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -Wall -Wextra -Wpedantic -Og -g>
  $<$<C_COMPILER_ID:MSVC>: -W4>
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(CTest)
  add_subdirectory(tests)

  target_compile_options(ds
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -fsanitize=address,undefined>
  )

  target_link_options(ds
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -fsanitize=address,undefined>
  )
endif()

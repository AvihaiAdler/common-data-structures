set(TESTS
  ascii_str_sanity
  bst_sanity
  ht_sanity
  ll_sanity
  vect_sanity
  pair_sanity
)

foreach(test ${TESTS})
  add_executable(${test})
  target_sources(${test}
    PRIVATE ${test}.c
  )

  add_test(NAME ${test} COMMAND $<TARGET_FILE:${test}>)

  target_compile_features(${test}
    PRIVATE c_std_99
  )

  target_compile_options(${test}
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -Wall -Wextra -Wpedantic -Og -g -fsanitize=address,undefined>
    $<$<C_COMPILER_ID:MSVC>: -W4 -fsanitize=address>
  )

  target_link_options(${test}
    PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>: -fsanitize=address,undefined>
    $<$<C_COMPILER_ID:MSVC>: -fsanitize=address>
  )

  target_link_libraries(${test}
    PRIVATE ds
  )
endforeach()

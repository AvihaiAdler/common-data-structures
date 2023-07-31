set(LL_UNIT_TESTS ll_sanity)

foreach(TEST ${LL_UNIT_TESTS})
  add_executable(${TEST} ${TEST}.c)
  add_test(NAME ${TEST} COMMAND ${PROJECT_BINARY_DIR}/tests/linked_list/${TEST})

  target_compile_options(${TEST} PRIVATE -Wall -Wextra -pedantic -fsanitize=address,undefined)
  target_link_options(${TEST} PRIVATE -fsanitize=address,undefined)

  target_link_libraries(${TEST} PRIVATE ds)
endforeach()
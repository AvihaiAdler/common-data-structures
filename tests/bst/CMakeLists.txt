set(BST_TEST bst_sanity)
foreach(TEST ${BST_TEST})
  add_executable(${TEST} ${TEST}.c)
  add_test(NAME ${TEST} COMMAND ${PROJECT_BINARY_DIR}/tests/bst/${TEST})

  target_compile_options(${TEST} PRIVATE -Wall -Wextra -Wpedantic -fsanitize=address,undefined)
  target_link_options(${TEST} PRIVATE -fsanitize=address,undefined)

  target_link_libraries(${TEST} PRIVATE generics)
  target_include_directories(${TEST} PRIVATE ${CMAKE_SOURCE_DIR})  
endforeach(TEST ${BST_TEST})

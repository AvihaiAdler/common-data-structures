set(VECTOR_UNIT_TESTS vect_sanity)

foreach(TEST ${VECTOR_UNIT_TESTS})
  
add_executable(${TEST} ${TEST}.c)
add_test(NAME ${TEST} COMMAND ${PROJECT_BINARY_DIR}/tests/vector/${TEST})

target_compile_options(${TEST} PRIVATE -Wall -Wextra -pedantic -fsanitize=address,undefined)
target_link_options(${TEST} PRIVATE -fsanitize=address,undefined)

target_include_directories(${TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${TEST} PRIVATE generics)


endforeach(TEST)

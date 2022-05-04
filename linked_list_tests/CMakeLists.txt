add_executable(ll_tests ll_tests.c)
target_include_directories(ll_tests PRIVATE ../include)
target_compile_options(ll_tests PUBLIC -Wall -Wextra -padantic -fsanitize=address,undefined)
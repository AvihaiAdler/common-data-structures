# configure the directories which include include files (.h)
# include_directories(
#   # src
#   include 
# )

# create a library called vector from src/vector_abstract.c
add_library(
  vector_lib
  src/vector_abstract.c
)

# configure the include files for others 
# target_include_directories(vector_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(vector_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

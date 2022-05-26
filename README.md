#### libgenerics
libgenerics is a small static library for personal use consists of 3 'generics' data structures written in c. The library contains an implementation of a vector, doubly linked list (aka list) and a hash table.

#### table of contents
- [vector]()
- [list]()
- [hash table]()

##### vector
Vector provides an implementation of a heap allocated vector. The underlying array saves a shallow copy of the data passed in. Note that you should avoid accessing the members of `struct vector` directly, use the various methods provided below.

- `struct vector *vector_init(unsigned long long data_size);`
  used to create a vector 'object'. `data_size` is the size of the data you want to store in bytes.
  ###### return value
  returns `struct vector *` on success, `NULL` on failure

- `void vector_destroy(struct vector *vector, void (*destroy)(void *element));`
  used to destroy a vector 'object'. expect a vector as well as a destroy function which may be `NULL`. you should olny pass a destroy function if your data either: contains a pointer, OR has more than 1 level of inderction (a double pointer for example)

- `unsigned long long vector_size(struct vector *vector);`
  used to get the number of elements the vector corrently holds.
  ###### return value
  the number of elements in the vector

- `unsigned long long vector_capacity(struct vector *vector);`
  used to get the number of elements the vector CAN hold.
  ###### return value
  the number of elements you can fit in the vector

##### list

##### hash table

#### building
You have 2 options when it comes to build and link your progect against the library:
1. Clone the repository. Create a subdirectory in the code base with your project in it and set Cmake accordingly
2. Download the `.a` file via packages or clone and build the project yourself. Copy the `.a` file into your project as well as the header files, and link the library against your project.
An example project will looks like:
```
|-project
  |-include
  | |-vector.h
  |-lib
  | |-libgenerics.a
  |-src
  | |-main.c
  |-CMakeLists.txt
```
CMakeLists.txt will then contains the following lines:
```
# import the library
add_library(libgenerics STATIC IMPORTED)
set_target_properties(
  libgenerics
  PROPERTIES
  IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libgenerics.a
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ds_example PRIVATE libgenerics)
```
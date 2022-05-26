#### libgenerics
libgenerics is a small static library for personal use consists of 3 'generics' data structures written in c. The library contains an implementation of a vector, doubly linked list (aka list) and a hash table.

#### table of contents
- [vector](https://github.com/AvihaiAdler/common-generic-data-structures#vector)
- [list](https://github.com/AvihaiAdler/common-generic-data-structures#list)
- [hash table](https://github.com/AvihaiAdler/common-generic-data-structures#hash-table)

##### vector
Vector provides an implementation of a heap allocated vector. The underlying array saves a shallow copy of the data passed in. Note that you should avoid accessing the members of `struct vector` directly, use the various methods provided below.

- ```c
  struct vector *vector_init(unsigned long long data_size);
  ```
  used to create a vector 'object'. `data_size` is the size of the data you want to store in bytes.
  ###### return value
    returns `struct vector *` on success, `NULL` on failure

- ```c
  void vector_destroy(struct vector *vector, void (*destroy)(void *element));
  ```
  used to destroy a vector 'object'. expects a vector as well as a destroy function which may be `NULL`. one should only pass a destroy function if your data either: contains a pointer, OR has more than 1 level of inderction (a double pointer for example).
  the destroy function should free any underlying data pointed to by `element`. note: you should never `free` `element` itself! however if your `element` is a double (or higher) pointer OR is a struct which contains a pointer to a heap allocated memory, you should `free` that data. e.g. `free(*(void **)element)`.
  

- ```c
  unsigned long long vector_size(struct vector *vector);
  ```
  used to get the number of elements the vector corrently holds.
  ###### return value
    returns the number of elements in the vector

- ```c
  unsigned long long vector_capacity(struct vector *vector);
  ```
  used to get the number of elements the vector CAN hold.
  ###### return value
    returns the number of elements you can fit in the vector

- ```c
  bool vector_empty(struct vector *vector);
  ```
  used as a method to determine whether a vector is empty or not.
  ###### return value
    returns `true` if the vector is empty or `NULL`, `false` otherwise

- ```c
  void *vector_at(struct vector *vector, unsigned long long pos);
  ```
  used to get a pointer to the element at position `pos`. the element must not be free'd.
  ###### return value
    returns a pointer to the element at position `pos`

- ```c
  void *vector_find(struct vector *vector, const void *element, int (*cmpr)(const void *, const void *));
  ```
  searches for an element `element` in the vector using the `cmpr` function. `element` is a pointer to the element one would like to find, `cmpr` is a compare function which should accepts 2 `const void *` and returns 0 if both are equal, 1 if the first is bigger than the second, and -1 if the first is smaller than the second.
  ###### return value
    returns a pointer to the desired element if exists or `NULL` if the element doesn't exists. the function may return `NULL` if either parameters passed in is `NULL`

- ```c
  unsigned long long vector_reserve(struct vector *vector, unsigned long long size);
  ```
  used to pre-allocate a space for `size` elements in the vector. note that the vector will allocate the space only if `size` is bigger than `vector::capacity`.
  ###### return value
    returns (the reserved space) `vector::capacity`

- ```c
  unsigned long long vector_resize(struct vector *vector, unsigned long long size);
  ```
  changes the size of the vector. if `size` < `vector::size`, `vector::size` will be decreased to the size passed in. beware if the vector contains a pointers to heap allocated memory you might loose track of them causing a memory leak. if `size` > `vector::capacity` the result will be as if `vector_reserve` were called followed by `vector_resize`. if `size` >= `vector::size` && `size` < `vector::capacity`, `vector::size` will be set to `size` and number of `NULL` values will be pushed into the vector.
  ###### return value
    returns (the new) `vector::size`

- ```c
  bool vector_push(struct vector *vector, const void *element);
  ```
  pushes an element into the vector. creates a shallow copy of the element passed in and saves that copy in the vector.
  ###### return value
    returns `true` on success, `false` otherwise

- ```c
  void *vector_pop(struct vector *vector);
  ```
  removes the last element from the end of vector and returns a pointer to it. the element must not be free'd unless the element contains a pointer to a heap allocated memeory or is itself a second (or higher) level of indirection to a heap allocated memory, at which case you should call your `destroy` function to free the data your element points to (note: you should ever free the element itself).
  ###### return value
    returns a pointer to the last element of the vector or `NULL` on failure

- ```c
  void *vector_remove_at(struct vector *vector, unsigned long long pos);
  ```
  removes an element at position `pos`. return a shallow copy of said element which has to be free'd 
  ###### return value
    returns the removed element (which has to be free'd) on success or `NULL` on failure

- ```c
  void *vector_replace(struct vector *vector, const void *element, unsigned long long pos);
  ```
  replaces an element at position `pos` with the new element `element`. return a shallow copy of the old element which has to be free'd 
  ###### return value
    returns the replaced element (which has to be free'd) on success or `NULL` on failure

- ```c
  unsigned long long vector_shrink(struct vector *vector);
  ```
  shrink the underlying array to fit exactly `vector::size` elements.
  ###### return value
    returns (the new) `vector::capacity`

- ```c
  long long vector_index_of(struct vector *vector, const void *element, int (*cmpr)(const void *, const void *));
  ```
  searches for the element `element` using the `cmpr` function. `element` is a pointer to the element one want to look for, `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, 1 if the first element is bigger than the second or -1 if the first element is smaller than the second.
  ###### return value
    returns the index of `element` on success or `N_EXISTS` on faliure

- ```c
  void vector_sort(struct vector *vector, int (*cmpr)(const void *, const void *));
  ```
  sorts the vector with the use of a `cmpr` function. `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, 1 if the first element is bigger than the second or -1 if the first element is smaller than the second.

##### list

##### hash table

#### building
You have 2 options when it comes to build and link your project against the library:
1. Clone the repository. Create a subdirectory in the code base with your project in it and set Cmake accordingly
2. Download the `.a` file via packages or clone and build the project yourself. Copy the `.a` file into your project as well as the header files, and link the library against your project.
An example project might looks like:
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
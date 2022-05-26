#### libgenerics
libgenerics is a small static library for personal use consists of 3 'generics' data structures written in c. The library contains an implementation of a vector, doubly linked list (aka list) and a hash table.

#### table of contents
- [vector](https://github.com/AvihaiAdler/common-generic-data-structures#vector)
- [list](https://github.com/AvihaiAdler/common-generic-data-structures#list)
- [hash table](https://github.com/AvihaiAdler/common-generic-data-structures#hash-table)
- [compiling and building](https://github.com/AvihaiAdler/common-generic-data-structures#compiling-and-building)

#### vector
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
  searches for an element `element` in the vector using the `cmpr` function. `element` is a pointer to the element one would like to find, `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.
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
  searches for the element `element` using the `cmpr` function. `element` is a pointer to the element one want to look for, `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.
  ###### return value
    returns the index of `element` on success or `N_EXISTS` on faliure

- ```c
  void vector_sort(struct vector *vector, int (*cmpr)(const void *, const void *));
  ```
  sorts the vector with the use of a `cmpr` function. `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.

#### list
List provides an implementation of a heap allocated, doubly linked list. Each node contains a shallow copy of the data one pass in. Note that you should avoid accessing the members of `struct list` directly, use the various methods provided below.

- ```c
  struct list *list_init();
  ```
  used to create a list 'object'.
  ###### return value
    returns a list 'object' on success, `NULL` on failure

- ```c
  void list_destroy(struct list *list, void (*destroy)(void *data));
  ```
  destroy a list `list`. accepts a destroy function which may be `NULL`. as a good practice one may not free `data` itself, but rather if `data` contains a pointer to a heap allocated memory one should free that said pointer.

- ```c
  unsigned long long list_size(struct list *list);
  ```
  used to get the number of elements the list currently holds.
  ###### return value
    returns the number of elements currently in the list

- ```c
  bool list_empty(struct list *list);
  ```
  used as a function to determine wether a list is empty or not.
  ###### return value
    returns `true` if `list` contains no elements or `NULL`, returns `false` otherwise

- ```c
  bool list_prepend(struct list *list, const void *data, unsigned long long data_size);
  ```
  adds a node with a shallow copy of `data` to the start of the list. expects a pointer to the data as well as `data_size` which is the size of `data` in bytes (e.g if you want to store an `int` -> `data_size = sizeof(int)` etc) (`O(1)`).
  ###### return value
    return `true` on success, `false` on failure

- ```c
  bool list_append(struct list *list, const void *data, unsigned long long data_size);
  ```
  adds a node with a shallow copy of `data` to the end of the list. expects a pointer to the data as well as `data_size` which is the size of `data` in bytes (e.g if you want to store an `int` -> `data_size = sizeof(int)` etc) (`O(1)`).
  ###### return value
    return `true` on success, `false` on failure

- ```c
  bool list_insert_at(struct list *list, const void *data, unsigned long long data_size, unsigned long long pos);
  ```
  inserts a node with a shallow copy of `data` at a given position `pos`. the position is calculated from the start of the list. expects a pointer to the data as well as `data_size` which is the size of `data` in bytes (e.g if you want to store an `int` -> `data_size = sizeof(int)` etc).
  ###### return value
    return `true` on success, `false` on failure

- ```c
  bool list_insert_priority(struct list *list, const void *data, unsigned long long data_size, int (*cmpr)(const void *, const void *));
  ```
  inserts a node with a shallow copy of `data` at a location which is determined by the `cmpr` function. the node will be inserted at the _first_ location where `node::data` is bigger than the next `other::data`. the comparison always starts from the start of the list. `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.
  ###### return value
    returns `true` on success, `false` on failure

- ```c
  void *list_peek_first(struct list *list);
  ```
  used to get the first element on the list without removing it (`O(1)`).
  ###### return value
    returns a pointer to the first element on the list, `NULL` on failure

- ```c
  void *list_peek_last(struct list *list);
  ```
  used to get the last element on the list without removing it (`O(1)`).
  ###### return value
    returns a pointer to the last element on the list, `NULL` on failure

- ```c
  void *list_at(struct list *list, unsigned long long pos);
  ```
  used to get the element at position `pos` whithout removing it. the position is calculated from the start of the list.
  ###### return value
    returns a pointer to the element at position `pos`, `NULL` on failure

- ```c
  void *list_remove_first(struct list *list);
  ```
  used to remove the first element on the list (`O(1)`).
  ###### return value
    returns a shallow copy of the removed element (which has to be free'd), `NULL` on failure

- ```c
  void *list_remove_last(struct list *list);
  ```
  used to remove the last element on the list (`O(1)`). 
  ###### return value
    returns a shallow copy of the removed element (which has to be free'd), `NULL` on failure

- ```c
  void *list_remove_at(struct list *list, unsigned long long pos);
  ```
  used to remove an element at position `pos`. the position is calculated from the start of the list.
  ###### return value
    returns a shallow copy of the removed element (which has to be free'd), `NULL` on failure

- ```c
  long long list_index_of(struct list *list, const void *data, int (*cmpr)(const void *, const void *));
  ```
  used to get the index of an element on the list. the index is calculated from the start of the list. `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.
  ###### return value
    returns the index of the desired element on success, returns `N_EXISTS` on failure

- ```c
  void *list_replace_at(struct list *list, const void *data, unsigned long long data_size, unsigned long long pos);
  ```
  replaces an element at position `pos` with the new element `data`. the position is calculated from the start of the list. `data_size` is the size of `data` in bytes (e.g if you want to store an `int` -> `data_size = sizeof(int)` etc).
  ###### return value
    returns a pointer to a shallow copy of the replaced element (which has to be free'd) on success, `NULL` on failure

- ```c
  void *list_replace(struct list *list, 
                     const void *old_data,
                     const void *new_data, 
                     unsigned long long new_data_size,
                     int (*cmpr)(const void *, const void *));
  ```
  replaces the element `old_data` with a new element `new_data`. `data_size` is the size of `data` in bytes (e.g if you want to store an `int` -> `data_size = sizeof(int)` etc). `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.
  ###### return value
    returns a pointer to a shallow copy of the replaced element (which has to be free'd) on success, `NULL` on failure

- ```c
  void list_sort(struct list *list, int (*cmpr)(const void *, const void *));
  ```
  sorts the list using the `cmpr` function. `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second.

#### hash table
Hash table provides an implementation of a heap allocated hash table. Under the hood the hash table consists of a vector which holds `size` entries. Each entry is a doubly linked list which contains a shallow copy of the data one might pass in. Note that you should avoid accessing the members of `struct hash_table` directly, use the various methods provided below.

- ```c
  struct hash_table *table_init(int (*cmpr)(const void *, const void *),
                                void (*destroy_key)(void *),
                                void (*destroy_value)(void *));
  ```
  used to create a `hash_table` 'object'. expects a `cmpr` function which must NOT be `NULL` as well as 2 destroy function which may be `NULL`. `cmpr` is a function which should accepts 2 `const void *` and return 0 if both are equal, a positive integer if the first element is bigger than the second or a negative integer if the first element is smaller than the second. as a good practice one may not free `key` or `value` themselves, but rather if `key` or `value` contains a pointer to a heap allocated memory one should free that said pointer.
  ###### return value
    returns a `struct hash_table *` on success, `NULL` on failure

- ```c
  void table_destroy(struct hash_table *table);
  ```
  destroys the hash table. if `destroy_key` or `destroy_value` aren't `NULL` calls them for every key-value pair.

- ```c
  bool table_empty(struct hash_table *table);
  ```
  used to determine if the table is empty or not.
  ###### return value
    returns `true` if the table contains no elements or if `table` is `NULL`, `false` otherwise

- ```c
  unsigned long long table_size(struct hash_table *table);
  ```
  used to get the number of elements the table currently holds.
  ###### return value
    returns the number of element currently stored in `table`

- ```c
  unsigned long long table_capacity(struct hash_table *table);
  ```
  used to get the number of entries in the table.
  ###### return value
    the number of enetries in the table 

- ```c
  void *table_put(struct hash_table *table, 
                  const void *key,
                  unsigned long long key_size, 
                  const void *value,
                  unsigned long long value_size);
  ```
  inserts an element into the table. `key` is a pointer to the key you want to associate with the data. `key` will determine the position of the data as `key` will be hashed. `key_size` is the size of `key` in bytes.
  `value` is a pointer to the data one want to store in the table. `value_size` is the size of `value` in bytes.
  the functions creates a shallow copy of `key` and `value` and inserts them into `table`.
  ###### return value
    returns a shallow copy of the old value associated with `key` (if such value exists) which has to be free'd, or `NULL` if no such value exists or if the function falied to insert the key-value pair

- ```c
  void *table_remove(struct hash_table *table, const void *key, unsigned long long key_size);
  ```
  removes the mapping for a specific `key` if present. `key_size` is the size of `key` in bytes.
  ###### return value
    returns a shallow copy of the removed value (whcih has to be free'd), or `NULL` on failure

- ```c
  void *table_get(struct hash_table *table, const void *key, unsigned long long key_size);
  ```
  returns the mapping for a specific `key` if present. `key_size` is the size of `key` in bytes.
  ###### return value
    returns a pointer to the value which is mapped to `key`. the value must NOT be free'd

#### compiling and building
You have 2 options when it comes to build and link your project against the library:
1. Clone the repository. Create a subdirectory in the code base with your project in it and set Cmake accordingly
2. Download the `.a` file via packages or clone and build the project yourself. Copy the `.a` file into your project as well as the header files, and link the library against your project.
An example project might looks like:
```bash
.
├── CMakeLists.txt
├── include
│   ├── hash_table.h
│   ├── list.h
│   └── vector.h
├── lib
│   └── libgenerics.a
└── src
    └── example.c
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
target_link_libraries(<executable name> PRIVATE libgenerics)
```
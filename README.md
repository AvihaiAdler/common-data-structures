#### libds
libgenerics is a small static library for personal use consists of 5 data structures written in C. The library contains an implementation of a vector, doubly linked list (aka list), a hash table, a binary search tree and a (ascii) string.

#### vector
Vector provides an implementation of a heap allocated vector. The underlying array saves a shallow copy of the data passed in.

#### list
List provides an implementation of a heap allocated, doubly linked list. Each node contains a shallow copy of the data one pass in. 

#### hash table
Hash table provides an implementation of a heap allocated hash table. Under the hood the hash table consists of a vector which holds `size` entries. Each entry is a doubly linked list which contains a shallow copy of the data one might pass in. 

#### compiling and building
The library uses CMake as its build system. As such one should has it installed. Building from source might look like:
`cmake -S <source directory> -B <build drectory> -G <generator> -DCMAKE_C_COMPILER=<compiler> -DCMAKE_BUILD_TYPE=<build type>`. 
followed by `cmake --build <build directory>`

For example: 
`cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build`

Invoking (the minimal) tests can be done by invoking the above 2 commands followed by
`ctest --test-dir build`

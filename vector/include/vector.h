#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>

#include "src/vector_abstract.h"

#define GENERATE_VECT(TYPE, NAME)                                              \
  struct Vector_##NAME {                                                       \
    size_t size;                                                               \
    size_t capacity;                                                           \
    size_t data_size;                                                          \
    TYPE *data;                                                                \
  };                                                                           \
                                                                               \
  struct Vector_##NAME *vect_##NAME##_init(void) {                             \
    return (struct Vector_##NAME *)vect_init(sizeof(TYPE));                    \
  }                                                                            \
                                                                               \
  TYPE *vect_##NAME##_data(struct Vector_##NAME *vector) {                     \
    return vect_data((void *)vector);                                          \
  }                                                                            \
                                                                               \
  TYPE vect_##NAME##_at(struct Vector_##NAME *vector, unsigned int index) {    \
    return vector->data[vect_at((void *)vector, index)];                       \
  }                                                                            \
                                                                               \
  void vect_##NAME##_push(struct Vector_##NAME *vector, TYPE value) {          \
    vector->data[vect_push((void *)vector)] = value;                           \
  }                                                                            \
                                                                               \
  TYPE vect_##NAME##_pop(struct Vector_##NAME *vector) {                       \
    return vector->data[vect_pop((void *)vector)];                             \
  }                                                                            \
                                                                               \
  void vect_##NAME##_replace(struct Vector_##NAME *vector, unsigned int index, \
                             TYPE value) {                                     \
    vector->data[vect_at((void *)vector, index)] = value;                      \
  }                                                                            \
                                                                               \
  long int vect_##NAME##_index_of(struct Vector_##NAME *vector, TYPE element,  \
                                  int (*comparator)(void *, void *)) {         \
    TYPE *data = vect_data((void *)vector);                                    \
    for (size_t pos = 0; pos < vector->size; pos++) {                          \
      if (!(*comparator)(&data[pos], &element)) return pos;                    \
    }                                                                          \
    return -1;                                                                 \
  }                                                                            \
                                                                               \
  void vect_##NAME##_sort(struct Vector_##NAME *vector,                        \
                          int (*comparator)(const void *, const void *)) {     \
    TYPE *data = vect_data((void *)vector);                                    \
    qsort(data, vector->size, vector->data_size, comparator);                  \
  }
#endif
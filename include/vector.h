#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 10
#define GROWTH_FACTOR 2

#define GENERATE_VECT(TYPE, NAME)                                              \
  struct NAME##_wrapper {                                                      \
    TYPE value;                                                                \
  };                                                                           \
                                                                               \
  struct NAME##_vector {                                                       \
    size_t size;                                                               \
    size_t capacity;                                                           \
    struct NAME##_wrapper *data;                                               \
  };                                                                           \
                                                                               \
  /* initialize a vector with an initial capacity of INIT_CAPACITY. return a   \
   * pointer to a heap allocated vector on success, NULL on failure */         \
  struct NAME##_vector *NAME##_vect_init(void) {                               \
    struct NAME##_vector *vect = calloc(1, sizeof *vect);                      \
    if (!vect) return NULL;                                                    \
                                                                               \
    vect->data = calloc(INIT_SIZE, sizeof *vect->data);                        \
    if (!vect->data) {                                                         \
      free(vect);                                                              \
      return NULL;                                                             \
    }                                                                          \
    vect->capacity = INIT_SIZE;                                                \
    vect->size = 0;                                                            \
    return vect;                                                               \
  }                                                                            \
                                                                               \
  /* destroy the vector. if the destroy function isn't NULL - calls it for     \
   * each vect->data[i] (struct NAME##_wrapper) */                             \
  void NAME##_vect_destroy(struct NAME##_vector *vect,                         \
                           void (*destroy)(TYPE elem)) {                       \
    if (!vect) return;                                                         \
    if (destroy) {                                                             \
      for (size_t pos = 0; pos < vect->size; pos++) {                          \
        destroy(vect->data[pos].value);                                        \
      }                                                                        \
    }                                                                          \
    free(vect->data);                                                          \
    free(vect);                                                                \
  }                                                                            \
                                                                               \
  /* initialize a heap allocated wrapper with the value value */               \
  struct NAME##_wrapper *NAME##_wrapper_init(TYPE value) {                     \
    struct NAME##_wrapper *wrapper = calloc(1, sizeof *wrapper);               \
    if (!wrapper) return NULL;                                                 \
                                                                               \
    memmove(&wrapper->value, &value, sizeof value);                            \
    return wrapper;                                                            \
  }                                                                            \
                                                                               \
  /* returns true if the vector is empty. false otherwise */                   \
  bool NAME##_vect_empty(struct NAME##_vector *vect) {                         \
    return vect->size == 0;                                                    \
  }                                                                            \
                                                                               \
  /* returns the address wrapper of the element at position pos. if pos >      \
   * vector->size - returns NULL. NOTE: the pointer returned is the pointer to \
   * the undelying data, any changes to it will change the data in the vector  \
   * itself. the pointer shouldn't be free'd */                                \
  struct NAME##_wrapper *NAME##_vect_at(struct NAME##_vector *vect,            \
                                        unsigned int pos) {                    \
    if (!vect) return NULL;                                                    \
    if (pos > vect->size) return NULL;                                         \
                                                                               \
    return &vect->data[pos];                                                   \
  }                                                                            \
                                                                               \
  /* resize the data a vector can hold by a factor of GROWTH_FACTOR and set    \
   * vector::capacity accordingly. return true on success, false otherwise */  \
  bool NAME##_vect_resize(struct NAME##_vector *vect) {                        \
    if (!vect) return false;                                                   \
                                                                               \
    struct NAME##_wrapper *tmp =                                               \
        calloc(vect->capacity * GROWTH_FACTOR, sizeof *vect->data);            \
    if (!tmp) return false;                                                    \
                                                                               \
    memmove(tmp, vect->data, vect->size * sizeof *vect->data);                 \
    free(vect->data);                                                          \
    vect->data = tmp;                                                          \
    vect->capacity *= GROWTH_FACTOR;                                           \
    return true;                                                               \
  }                                                                            \
                                                                               \
  /* reserve a space for new_capacity elements. returns the new reserved space \
   * (vect::capacity). if new_capacity <= vect::capacity returns               \
   * vect::capacity. if vect is NULL, returns 0 */                             \
  size_t NAME##_vect_reserve(struct NAME##_vector *vect,                       \
                             size_t new_capacity) {                            \
    if (!vect) return 0;                                                       \
                                                                               \
    if (new_capacity <= vect->capacity) return vect->capacity;                 \
    NAME##_vect_resize(vect);                                                  \
    return vect->capacity;                                                     \
  }                                                                            \
                                                                               \
  /* shrink vect::capacity to the size of vector::size. returns true on        \
   * success, false otherwise */                                               \
  bool NAME##_vect_shrink_to_fit(struct NAME##_vector *vect) {                 \
    if (!vect) return false;                                                   \
                                                                               \
    struct NAME##_wrapper *tmp = realloc(vect->data, vect->size);              \
    if (!tmp) return false;                                                    \
                                                                               \
    vect->data = tmp;                                                          \
    vect->capacity = vect->size;                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  /* push an element into the vector. returns true on success, false otherwise \
   */                                                                          \
  bool NAME##_vect_push(struct NAME##_vector *vect, TYPE value) {              \
    if (!vect) return false;                                                   \
                                                                               \
    if (vect->size == vect->capacity) {                                        \
      if (!NAME##_vect_resize(vect)) return false;                             \
    }                                                                          \
                                                                               \
    struct NAME##_wrapper *elem = NAME##_wrapper_init(value);                  \
    if (!elem) return false;                                                   \
                                                                               \
    memmove(&vect->data[vect->size].value, &elem->value, sizeof elem->value);  \
    free(elem);                                                                \
    vect->size++;                                                              \
    return true;                                                               \
  }                                                                            \
                                                                               \
  /* pop an element and returns its wrapper, the wrapper is heap allocated and \
   * must be free'd (if the element (wrapper::value) is heap allocated as well \
   * it must be free'd). returns NULL if the vector is empty or on wrapper     \
   * allocation failure                                                        \
   */                                                                          \
  struct NAME##_wrapper *NAME##_vect_pop(struct NAME##_vector *vect) {         \
    if (!vect) return NULL;                                                    \
    if (!vect->size) return NULL;                                              \
                                                                               \
    vect->size--;                                                              \
    struct NAME##_wrapper *elem =                                              \
        NAME##_wrapper_init(vect->data[vect->size].value);                     \
    if (!elem) {                                                               \
      vect->size++;                                                            \
      return NULL;                                                             \
    }                                                                          \
    return elem;                                                               \
  }                                                                            \
                                                                               \
  /* replaces an element at position pos. returns a wrapper with the old       \
   * element on success. the wrapper is heap allocated and must be free'd (if  \
   * its underlying data is heap allocated it must be free'd as well), returns \
   * NULL on failure */                                                        \
  struct NAME##_wrapper *NAME##_vect_replace(struct NAME##_vector *vect,       \
                                             unsigned int pos, TYPE value) {   \
    if (!vect) return NULL;                                                    \
    if (pos > vect->size) return NULL;                                         \
                                                                               \
    struct NAME##_wrapper *tmp = NAME##_wrapper_init(value);                   \
    struct NAME##_wrapper *elem = NAME##_wrapper_init(vect->data[pos].value);  \
    if (!tmp || !elem) {                                                       \
      if (tmp) free(tmp);                                                      \
      if (elem) free(elem);                                                    \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    memmove(&vect->data[pos].value, &tmp->value, sizeof tmp->value);           \
    free(tmp);                                                                 \
                                                                               \
    return elem;                                                               \
  }                                                                            \
                                                                               \
  /* check the vector for an element 'element' using an equal_to function.     \
   * returns the index of the found element or -1 if no such element found.    \
   * the equal_to must accept 2 elements of type TYPE, which return true if    \
   * both TYPEs are equal or false otherwise */                                \
  int64_t NAME##_vect_index_of(struct NAME##_vector *vect, TYPE element,       \
                               bool (*equal_to)(TYPE, TYPE)) {                 \
    if (!vect) return -1;                                                      \
                                                                               \
    for (size_t pos = 0; pos < vect->size; pos++) {                            \
      if (equal_to(vect->data[pos].value, element)) return pos;                \
    }                                                                          \
    return -1;                                                                 \
  }                                                                            \
                                                                               \
  /* sort the vector according to the comparator passed in. the comparator     \
   * must return 0 if both elements are equal, any value bigger than 0 if the  \
   * first element if bigger than the second or any value less than 0 if the   \
   * first element if smaller than the second */                               \
  void NAME##_vect_sort(struct NAME##_vector *vect,                            \
                        int (*comparator)(const void *, const void *)) {       \
    if (!vect) return;                                                         \
    qsort(vect->data, vect->size, sizeof *vect->data, comparator);             \
  }

#endif
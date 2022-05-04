#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 10
#define GROWTH_FACTOR 2

enum err_val {
  INVAL = 1,
  RANGE,
  ACCESS,
};

static int errno_t = 0;

#define GENERATE_VECT(TYPE, NAME)                                              \
  struct NAME##_vector {                                                       \
    size_t size;                                                               \
    size_t capacity;                                                           \
    TYPE *data;                                                                \
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
  /*destroy the vector. if the destroy function isn't NULL - calls it for each \
   * vect->data[i] */                                                          \
  void NAME##_vect_destroy(struct NAME##_vector *vect,                         \
                           void (*destroy)(TYPE elem)) {                       \
    if (!vect) return;                                                         \
    if (destroy) {                                                             \
      for (size_t pos = 0; pos < vect->size; pos++) {                          \
        destroy(vect->data[pos]);                                              \
      }                                                                        \
    }                                                                          \
    free(vect->data);                                                          \
  }                                                                            \
                                                                               \
  /* returns true if the vector is empty. false otherwise */                   \
  bool NAME##_vect_empty(struct NAME##_vector *vect) {                         \
    return vect->size == 0;                                                    \
  }                                                                            \
                                                                               \
  /* returns the element at position pos. if pos > vector->size - returns 0    \
   * and set errno to EINVAL */                                                \
  TYPE NAME##_vect_at(struct NAME##_vector *vect, unsigned int pos) {          \
    if (!vect) {                                                               \
      errno_t = INVAL;                                                         \
      return 0;                                                                \
    }                                                                          \
    if (pos > vect->size) {                                                    \
      errno_t = RANGE;                                                         \
      return 0;                                                                \
    }                                                                          \
    errno_t = 0;                                                               \
    return vect->data[pos];                                                    \
  }                                                                            \
                                                                               \
  /* resize the data a vector can hold by a factor of GROWTH_FACTOR and set    \
   * vector::capacity accordingly. retun true on success, false otherwise */   \
  bool NAME##_vect_resize(struct NAME##_vector *vect) {                        \
    if (!vect) return false;                                                   \
                                                                               \
    TYPE *tmp = calloc(vect->capacity * GROWTH_FACTOR, sizeof *vect->data);    \
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
   * vect::capacity. if vect is NULL, set errno_t to INVAL and returns 0 */    \
  size_t NAME##_vect_reserve(struct NAME##_vector *vect,                       \
                             size_t new_capacity) {                            \
    if (!vect) {                                                               \
      errno_t = INVAL;                                                         \
      return 0;                                                                \
    }                                                                          \
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
    TYPE *tmp = realloc(vect->data, vect->size);                               \
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
    vect->data[vect->size++] = value;                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  /* pop en element and returns it (if the element is heap allocated it must   \
   * be free'd). set errno to EINVAL if the vector is empty                    \
   */                                                                          \
  TYPE NAME##_vect_pop(struct NAME##_vector *vect) {                           \
    if (!vect) {                                                               \
      errno_t = INVAL;                                                         \
      return 0;                                                                \
    }                                                                          \
    if (!vect->size) {                                                         \
      errno_t = INVAL;                                                         \
      return 0;                                                                \
    }                                                                          \
    errno_t = 0;                                                               \
    return vect->data[--vect->size];                                           \
  }                                                                            \
                                                                               \
  /* replaces an element at position pos. returns the old element on success   \
   * (if it's heap allocated it must be free'd), set errno to EAINVAL on       \
   * failure */                                                                \
  TYPE NAME##_vect_replace(struct NAME##_vector *vect, unsigned int pos,       \
                           TYPE value) {                                       \
    if (!vect) {                                                               \
      errno_t = INVAL;                                                         \
      return 0;                                                                \
    }                                                                          \
    if (pos > vect->size) {                                                    \
      errno_t = RANGE;                                                         \
      return 0;                                                                \
    }                                                                          \
    errno_t = 0;                                                               \
    TYPE tmp = vect->data[pos];                                                \
    vect->data[pos] = value;                                                   \
    return tmp;                                                                \
  }                                                                            \
                                                                               \
  /* check the vector of an element element using an equal_to function.        \
   * returns the index of the found element or -1 if no such element found.    \
   * returns -1 if vect if NULL and set errno_t to INVAL. the equal_to must    \
   * accept 2 elements of type TYPE, return true if both are equal or false    \
   * otherwise */                                                              \
  int64_t NAME##_vect_index_of(struct NAME##_vector *vect, TYPE element,       \
                               bool (*equal_to)(TYPE, TYPE)) {                 \
    if (!vect) {                                                               \
      errno_t = INVAL;                                                         \
      return -1;                                                               \
    }                                                                          \
    for (size_t pos = 0; pos < vect->size; pos++) {                            \
      if (equal_to(vect->data[pos], element)) return pos;                      \
    }                                                                          \
    return -1;                                                                 \
  }                                                                            \
                                                                               \
  /* sort the vector according to the comparator passed in. set errno_t to     \
   * INVAL if vect is NULL. the comparator must return 0 if both elements are  \
   * equal, any value bigger than 0 if the first element if bigger than the    \
   * second or any value less than 0 if the first elrment if smaller than the  \
   * second */                                                                 \
  void NAME##_vect_sort(struct NAME##_vector *vect,                            \
                        int (*comparator)(const void *, const void *)) {       \
    if (!vect) {                                                               \
      errno_t = INVAL;                                                         \
      return;                                                                  \
    }                                                                          \
    qsort(vect->data, vect->size, sizeof *vect->data, comparator);             \
  }

#endif
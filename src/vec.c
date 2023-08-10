#include "vec.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct vec vec_create(size_t data_size, void (*destroy)(void *_element)) {
  // limit check
  if (data_size == 0) goto empty_vec;
  if ((SIZE_MAX >> 1) / data_size < VECT_INIT_CAPACITY) goto empty_vec;

  void *data = calloc(VECT_INIT_CAPACITY * data_size, 1);
  if (!data) goto empty_vec;

  return (struct vec){._capacity = VECT_INIT_CAPACITY,
                      ._destroy = destroy,
                      ._data = data,
                      ._data_size = data_size,
                      ._n_elem = 0};

empty_vec:
  return (struct vec){0};
}

void vec_destroy(struct vec *vec) {
  if (!vec || !vec->_data) return;

  if (vec->_destroy) {
    for (size_t i = 0; i < vec->_n_elem * vec->_data_size; i += vec->_data_size) {
      vec->_destroy((char *)vec->_data + i);
    }
  }
  free(vec->_data);
}

size_t vec_size(struct vec const *vec) {
  return vec ? vec->_n_elem : 0;
}

size_t vec_capacity(struct vec const *vec) {
  return vec ? vec->_capacity : 0;
}

void *vec_data(struct vec *vec) {
  return vec ? vec->_data : NULL;
}

bool vec_empty(struct vec const *vec) {
  return vec ? vec->_n_elem == 0 : true;
}

void *vec_at(struct vec *vec, size_t pos) {
  if (!vec || !vec->_data) return NULL;
  if (pos >= vec->_n_elem) return NULL;

  return (char *)vec->_data + (pos * vec->_data_size);
}

void *vec_find(struct vec *restrict vec, void const *restrict element, int (*cmpr)(void const *_a, void const *_b)) {
  if (!vec || !vec->_data) return NULL;
  if (!cmpr) return NULL;

  for (size_t i = 0; i < vec->_n_elem; i++) {
    int ret = cmpr(element, (char *)vec->_data + (i * vec->_data_size));
    if (ret == 0) return vec_at(vec, i);
  }

  return NULL;
}

/* used internally to resize the vec by GROWTH_FACTOR */
static bool vec_resize_internal(struct vec *vec) {
  // limit check. vec:capacity cannot exceeds (SIZE_MAX >> 1)
  if ((SIZE_MAX >> 1) >> GROWTH_FACTOR < vec->_capacity) return false;
  size_t new_capacity = vec->_capacity << GROWTH_FACTOR;

  // limit check. vec::capacity * vec::data_size (the max number of
  // element the vec can hold) cannot exceeds (SIZE_MAX >> 1) / vec::data_size
  // (the number of elements (SIZE_MAX >> 1) can hold)
  if ((SIZE_MAX >> 1) / vec->_data_size < new_capacity) return false;

  void *tmp = realloc(vec->_data, new_capacity * vec->_data_size);
  if (!tmp) return false;

  memset((char *)tmp + vec->_n_elem * vec->_data_size,
         0,
         new_capacity * vec->_data_size - vec->_n_elem * vec->_data_size);

  vec->_capacity = new_capacity;
  vec->_data = tmp;
  return true;
}

size_t vec_reserve(struct vec *vec, size_t count) {
  if (!vec || !vec->_data) return 0;

  size_t free_elem = vec->_capacity - vec->_n_elem;
  if (count <= free_elem) return vec->_capacity;

  size_t new_capacity = vec->_capacity + count - free_elem;
  if (new_capacity * vec->_data_size > (SIZE_MAX >> 1)) return vec->_capacity;

  void *tmp = realloc(vec->_data, new_capacity * vec->_data_size);
  if (!tmp) return vec->_capacity;

  memset((char *)tmp + vec->_n_elem * vec->_data_size, 0, (new_capacity - vec->_n_elem) * vec->_data_size);

  vec->_capacity = new_capacity;
  vec->_data = tmp;
  return vec->_capacity;
}

size_t vec_resize(struct vec *vec, size_t num_elements) {
  if (!vec || !vec->_data) return 0;

  if (num_elements > vec->_capacity) {
    if (num_elements * vec->_data_size > (SIZE_MAX >> 1)) return vec->_capacity;

    void *tmp = realloc(vec->_data, num_elements * vec->_data_size);
    if (!tmp) return vec->_capacity;

    memset((char *)tmp + vec->_n_elem * vec->_data_size, 0, (num_elements - vec->_n_elem) * vec->_data_size);
    vec->_data = tmp;
    vec->_capacity = num_elements;
  }

  return vec->_capacity;
}

bool vec_push(struct vec *restrict vec, void const *restrict element) {
  if (!vec || !vec->_data) return false;
  if (vec->_n_elem == vec->_capacity) {
    if (!vec_resize_internal(vec)) return false;
  }

  memcpy((char *)vec->_data + vec->_n_elem * vec->_data_size, element, vec->_data_size);
  vec->_n_elem++;
  return true;
}

void const *vec_pop(struct vec *vec) {
  if (!vec || !vec->_data) return NULL;

  return (char *)vec->_data + (--vec->_n_elem) * vec->_data_size;
}

void *vec_remove_at(struct vec *vec, size_t pos) {
  void *tmp = vec_at(vec, pos);
  if (!tmp) return NULL;

  void *old = calloc(1, vec->_data_size);
  if (!old) return NULL;

  memcpy(old, tmp, vec->_data_size);

  memmove((char *)vec->_data + pos * vec->_data_size,
          (char *)vec->_data + (pos + 1) * vec->_data_size,
          (vec->_n_elem - pos - 1) * vec->_data_size);
  vec->_n_elem--;
  return old;
}

void *vec_replace(struct vec *restrict vec, void const *restrict element, size_t pos) {
  void *tmp = vec_at(vec, pos);
  if (!tmp) return NULL;

  void *old = calloc(1, vec->_data_size);
  if (!old) return NULL;

  memcpy(old, tmp, vec->_data_size);

  memcpy((char *)vec->_data + (pos * vec->_data_size), element, vec->_data_size);
  return old;
}

size_t vec_shrink(struct vec *vec) {
  if (!vec || !vec->_data) return 0;

  size_t new_capacity = vec->_n_elem;
  void *tmp = realloc(vec->_data, new_capacity * vec->_data_size);
  if (!tmp) return vec->_capacity;

  vec->_capacity = new_capacity;
  vec->_data = tmp;
  return vec->_capacity;
}

void vec_sort(struct vec *vec, int (*cmpr)(void const *_a, void const *_b)) {
  if (!vec || !vec->_data) return;
  if (!cmpr) return;

  qsort(vec->_data, vec->_n_elem, vec->_data_size, cmpr);
}

void *vec_iter_begin(struct vec *vec) {
  if (!vec) return NULL;

  return vec_empty(vec) ? vec_iter_end(vec) : vec->_data;
}

void *vec_iter_end(struct vec *vec) {
  if (!vec) return NULL;

  return (char *)vec->_data + vec->_n_elem * vec->_data_size;
}

void *vec_iter_next(struct vec *restrict vec, void *restrict iter) {
  if (!vec || !iter) return NULL;

  return vec_empty(vec) ? vec_iter_end(vec) : (char *)iter + vec->_data_size;
}

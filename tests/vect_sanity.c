#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "vec.h"

static int cmpr(const void *a, const void *b) {
  const int *i_a = a;
  const int *i_b = b;
  return (*i_a > *i_b) - (*i_a < *i_b);
}

struct vec before(int *arr, size_t arr_size) {
  struct vec vect = vec_create(sizeof *arr, NULL);
  for (size_t i = 0; i < arr_size; i++) {
    assert(vec_push(&vect, arr + i));
  }
  return vect;
}

static void after(struct vec *vect) {
  vec_destroy(vect);
}

static void vec_push_sanity_test(int num) {
  // given
  struct vec vect = vec_create(sizeof num, NULL);
  assert(vec_empty(&vect));

  // when
  bool res = vec_push(&vect, &num);

  // then
  assert(res);
  int *stored = vec_at(&vect, 0);
  assert(!vec_empty(&vect));
  assert(stored);
  assert(*stored == num);

  // cleanup
  after(&vect);
}

static void vec_pop_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);

  assert(vec_size(&vect) == arr_size);

  // when
  const int *poped = vec_pop(&vect);

  // then
  assert(vec_size(&vect) == arr_size - 1);
  assert(poped);
  assert(*poped == arr[arr_size - 1]);

  // cleanup
  after(&vect);
}

static void vec_at_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);

  // when
  int *first = vec_at(&vect, 0);
  int *last = vec_at(&vect, arr_size - 1);
  int *middle = vec_at(&vect, arr_size / 2);

  // then
  assert(first);
  assert(last);
  assert(middle);
  assert(*first == *arr);
  assert(*last == arr[arr_size - 1]);
  assert(*middle == arr[arr_size / 2]);

  // cleanup
  after(&vect);
}

static void vec_find_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);

  // when
  int original = arr[arr_size / 2];
  int *elem = vec_find(&vect, &original, cmpr);

  // then
  assert(elem);
  assert(*elem == original);

  // cleanup
  after(&vect);
}

static void vec_reserve_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);

  size_t init_capacity = vec_capacity(&vect);
  size_t init_n_elem = vec_size(&vect);
  assert(init_capacity > 0);

  // when
  size_t new_capacity = vec_reserve(&vect, init_capacity * 4);

  // then
  assert(new_capacity > init_capacity);
  assert(new_capacity == init_capacity * 4 + init_n_elem);

  // cleanup
  after(&vect);
}

static void vec_remove_at_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);
  assert(vec_size(&vect) == arr_size);

  // when
  int *removed = vec_remove_at(&vect, arr_size / 2);

  // then
  assert(removed);
  assert(*removed == arr[arr_size / 2]);
  assert(vec_find(&vect, &arr[arr_size / 2], cmpr) == NULL);

  // cleanup
  free(removed);
  after(&vect);
}

static void vec_replace_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);
  int num = -1;

  // when
  int *replaced = vec_replace(&vect, &num, arr_size / 2);

  // then
  assert(replaced);
  assert(*replaced == arr[arr_size / 2]);
  assert(vec_find(&vect, arr + arr_size / 2, cmpr) == NULL);

  // cleanup
  free(replaced);
  after(&vect);
}

static void vec_shrink_sanity_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);
  size_t init_capacity = vec_capacity(&vect);
  size_t vect_init_size = vec_size(&vect);

  // force a resize
  if (vect_init_size == init_capacity) {
    vec_push(&vect, &arr[0]);
    init_capacity = vec_capacity(&vect);
  }

  // when
  size_t new_capacity = vec_shrink(&vect);
  size_t vect_size = vec_size(&vect);

  // then
  assert(init_capacity > new_capacity);
  assert(new_capacity == vect_size);

  // cleanup
  after(&vect);
}

static void vec_sort_santiy_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);

  // when
  vec_sort(&vect, cmpr);

  // then
  size_t vect_size = vec_size(&vect);
  for (size_t i = 0; i < vect_size - 1; i++) {
    assert(*(int *)vec_at(&vect, i) < *(int *)vec_at(&vect, i + 1));
  }

  // cleanup
  after(&vect);
}

static void vec_iter_empty_vec_test(void) {
  // given
  struct vec vect = vec_create(sizeof(int), NULL);

  // when
  void *begin = vec_iter_begin(&vect);
  void *end = vec_iter_end(&vect);

  // then
  assert(begin == end);
  assert(vec_iter_next(&vect, begin) == end);

  // cleanup
  after(&vect);
}

static void vec_iter_forward_iteration_test(int *arr, size_t arr_size) {
  // given
  struct vec vect = before(arr, arr_size);

  // when
  void *begin = vec_iter_begin(&vect);
  void *end = vec_iter_begin(&vect);

  // then
  for (size_t i = 0; i < arr_size && begin != end; i++, begin = vec_iter_next(&vect, begin)) {
    int *elem = begin;
    assert(*elem == arr[i]);
  }

  // cleanup
  after(&vect);
}

static void vec_resize_test(void) {
  struct vec vec = vec_create(1, NULL);

  char *ptr = vec_data(&vec);
  assert(ptr);

  for (size_t i = 0; i < vec_capacity(&vec); i++) {
    assert(ptr[i] == 0);
  }

  size_t old_capacity = vec_capacity(&vec);
  assert(vec_resize(&vec, old_capacity * 2) == old_capacity * 2);

  ptr = vec_data(&vec);
  for (size_t i = 0; i < vec_capacity(&vec); i++) {
    assert(ptr[i] == 0);
  }

  vec_destroy(&vec);
}

static void populate_array(int *arr, size_t arr_size) {
  for (size_t i = 0; i < arr_size; i++) {
    arr[arr_size - i - 1] = i;
  }
}

int main(void) {
  enum size {
    SIZE = 200000,
  };

  int arr[SIZE];
  size_t arr_size = sizeof arr / sizeof *arr;
  populate_array(arr, arr_size);

  vec_push_sanity_test(1);
  vec_pop_sanity_test(arr, arr_size);
  vec_at_sanity_test(arr, arr_size);
  vec_find_sanity_test(arr, arr_size);
  vec_reserve_sanity_test(arr, arr_size);
  vec_remove_at_sanity_test(arr, arr_size);
  vec_replace_sanity_test(arr, arr_size);
  vec_shrink_sanity_test(arr, arr_size);
  vec_sort_santiy_test(arr, arr_size);
  vec_resize_test();

  vec_iter_empty_vec_test();
  vec_iter_forward_iteration_test(arr, arr_size);
  return 0;
}

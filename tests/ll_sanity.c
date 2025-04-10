#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ascii_str.h"
#include "list.h"

#define LOG(file, fmt, ...) \
  do { fprintf(file, "%s %s:%d\n\t" fmt, __FILE__, __func__, __LINE__, __VA_ARGS__); } while (0);

static int generate_random(int min, int max) {
  int range = max - min;
  double rand_val = rand() / (1.0 + RAND_MAX);
  return (int)(rand_val * range + min);
}

static void destroy_string(void *_string) {
  struct ascii_str *string = _string;

  ascii_str_destroy(string);
}

static struct ascii_str rand_string(size_t len) {
  char const *alphabet = "abcdefghijklmnopqrstuvwxyz";
  size_t alphabet_len = strlen(alphabet);

  struct ascii_str str = ascii_str_create(NULL, 0);
  for (size_t i = 0; i < len; i++) {
    int rand_idx = generate_random(0, alphabet_len);
    ascii_str_push(&str, alphabet[rand_idx]);
  }

  return str;
}

static struct list before(struct ascii_str *strings, size_t size) {
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);
  for (size_t i = 0; i < size; i++) {
    int rand_str_len = generate_random(1, 100);

    struct ascii_str str = rand_string((size_t)rand_str_len);
    list_prepend(&list, &str);
    strings[size - i - 1] = str;
  }

  return list;
}

static void after(struct list *list) {
  list_destroy(list);
}

static int cmpr(void const *_a, void const *_b) {
  // necessary evil
  struct ascii_str *a = (void *)_a;
  struct ascii_str *b = (void *)_b;

  return strcmp(ascii_str_c_str(a), ascii_str_c_str(b));
}

static void list_prepend_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);

  // when
  for (size_t i = 0; i < size; i++) {
    struct ascii_str str = rand_string(10);
    assert(list_prepend(&list, &str));

    strings[size - i - 1] = str;
  }

  // then
  assert(list_size(&list) == size);
  assert(!list_empty(&list));

  struct ascii_str *first = list_peek_first(&list);
  struct ascii_str *last = list_peek_last(&list);
  assert(cmpr(first, strings) == 0);
  assert(cmpr(last, strings + size - 1) == 0);

  // cleanup
  list_destroy(&list);
}

static void list_append_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);

  // when
  for (size_t i = 0; i < size; i++) {
    struct ascii_str str = rand_string(10);
    assert(list_append(&list, &str));

    strings[i] = str;
  }

  // then
  assert(list_size(&list) == size);
  assert(!list_empty(&list));

  struct ascii_str *first = list_peek_first(&list);
  struct ascii_str *last = list_peek_last(&list);
  assert(cmpr(first, strings) == 0);
  assert(cmpr(last, strings + size - 1) == 0);

  // cleanup
  list_destroy(&list);
}

static void list_insert_at_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);

  // when
  struct ascii_str str = rand_string(10);
  size_t pos = generate_random(0, size - 1);

  LOG(stderr, "inserting [%s] at index %zu\n", ascii_str_c_str(&str), pos);
  assert(list_insert_at(&list, &str, pos));

  // then
  assert(list_size(&list) == size + 1);
  assert(cmpr(&str, list_at(&list, pos)) == 0);

  // cleanup
  after(&list);
}

static void list_peek_first_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);

  // when
  struct ascii_str *str = list_peek_first(&list);

  // then
  assert(str == NULL);
  assert(list_empty(&list));
  assert(list_size(&list) == 0);

  // cleanup
  list_destroy(&list);
}

static void list_peek_first_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);

  // when
  struct ascii_str *str = list_peek_first(&list);

  // then
  assert(cmpr(str, strings) == 0);

  // cleanup
  after(&list);
}

static void list_peek_last_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);

  // when
  struct ascii_str *str = list_peek_last(&list);

  // then
  assert(str == NULL);
  assert(list_empty(&list));
  assert(list_size(&list) == 0);

  // cleanup
  list_destroy(&list);
}

static void list_peek_last_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);

  // when
  struct ascii_str *str = list_peek_last(&list);

  // then
  assert(cmpr(str, strings + size - 1) == 0);

  // cleanup
  after(&list);
}

static void list_at_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);

  // when
  void *first = list_at(&list, 0);
  void *rand = list_at(&list, (size_t)generate_random(1, 100));

  // then
  assert(!first && !rand);

  // cleanup
  list_destroy(&list);
}

static void list_at_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);

  // when
  struct ascii_str *first = list_at(&list, 0);
  struct ascii_str *last = list_at(&list, size - 1);
  struct ascii_str *middle = list_at(&list, size / 2);
  struct ascii_str *one_past_last = list_at(&list, size);

  // then
  assert(!one_past_last);
  assert(cmpr(first, strings) == 0);
  assert(cmpr(last, strings + size - 1) == 0);
  assert(cmpr(middle, strings + size / 2) == 0);

  // cleanup
  after(&list);
}

static void list_remove_first_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);
  size_t size = list_size(&list);

  // when
  void *ptr = list_remove_first(&list);

  // then
  assert(size == list_size(&list));
  assert(!ptr);

  // cleanup
  list_destroy(&list);
}

static void list_remove_first_test(struct ascii_str *strings, size_t size) {
  assert(size);
  // given
  struct list list = before(strings, size);
  size_t pre_size = list_size(&list);

  // when
  void *first = list_remove_first(&list);
  // then
  assert(cmpr(first, strings) == 0);

  if (size > 1) {
    void *second = list_remove_first(&list);

    assert(pre_size - 2 == list_size(&list));
    assert(cmpr(second, strings + 1) == 0);
    ascii_str_destroy(second);
    free(second);
  }

  // cleanup
  ascii_str_destroy(first);

  free(first);

  after(&list);
}

static void list_remove_last_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);
  size_t size = list_size(&list);

  // when
  void *ptr = list_remove_last(&list);

  // then
  assert(size == list_size(&list));
  assert(!ptr);

  // cleanup
  list_destroy(&list);
}

static void list_remove_last_test(struct ascii_str *strings, size_t size) {
  assert(size >= 2);

  // given
  struct list list = before(strings, size);
  size_t pre_size = list_size(&list);

  // when
  void *last = list_remove_last(&list);
  void *second_to_last = list_remove_last(&list);

  // then
  assert(pre_size - 2 == list_size(&list));
  assert(cmpr(last, strings + size - 1) == 0);
  assert(cmpr(second_to_last, strings + size - 2) == 0);

  // cleanup
  ascii_str_destroy(last);
  ascii_str_destroy(second_to_last);

  free(last);
  free(second_to_last);

  after(&list);
}

static void list_remove_at_rand_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);
  size_t pos = (size_t)generate_random(0, 100);

  // when
  LOG(stderr, "attempting to remove a string at index: %zu\n", pos);
  void *ptr = list_remove_at(&list, pos);

  // then
  assert(!ptr);

  list_destroy(&list);
}

static void list_remove_at_zero_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);
  size_t pos = 0;

  // when
  LOG(stderr, "attempting to remove a string at index: %zu\n", pos);
  void *ptr = list_remove_at(&list, pos);

  // then
  assert(!ptr);

  list_destroy(&list);
}

static void list_remove_at_rand_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);
  size_t pos = (size_t)generate_random(0, size);

  // when
  LOG(stderr, "attempting to remove a string at index: %zu\n", pos);
  void *ptr = list_remove_at(&list, pos);

  // then
  assert(list_size(&list) == size - 1);
  assert(cmpr(ptr, strings + pos) == 0);

  // cleanup
  ascii_str_destroy(ptr);
  free(ptr);
  after(&list);
}

static void list_remove_at_zero_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);
  size_t pos = 0;

  // when
  LOG(stderr, "attempting to remove a string at index: %zu\n", pos);
  void *ptr = list_remove_at(&list, pos);

  // then
  assert(list_size(&list) == size - 1);
  assert(cmpr(ptr, strings + pos) == 0);

  // cleanup
  ascii_str_destroy(ptr);
  free(ptr);
  after(&list);
}

static void list_remove_at_size_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);
  size_t pos = size - 1;

  // when
  LOG(stderr, "attempting to remove a string at index: %zu\n", pos);
  void *ptr = list_remove_at(&list, pos);

  // then
  assert(list_size(&list) == size - 1);
  assert(cmpr(ptr, strings + pos) == 0);

  // cleanup
  ascii_str_destroy(ptr);
  free(ptr);
  after(&list);
}

static void list_replace_at_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);
  struct ascii_str str = rand_string(10);
  size_t pos = (size_t)generate_random(1, 100);

  // when
  LOG(stderr, "attempting to replace a string at index: %d\n", 0);
  void *first = list_replace_at(&list, &str, 0);

  LOG(stderr, "attempting to replace a string at index: %zu\n", pos);
  void *rand = list_replace_at(&list, &str, pos);

  // then
  assert(list_empty(&list));
  assert(list_size(&list) == 0);
  assert(!first);
  assert(!rand);

  ascii_str_destroy(&str);
  list_destroy(&list);
}

static void list_replace_at_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);
  struct ascii_str str = rand_string(10);

  // when
  LOG(stderr, "attempting to replace a string at index: %d\n", 0);
  void *first = list_replace_at(&list, &str, 0);

  LOG(stderr, "attempting to replace a string at index: %zu\n", size / 2);
  void *middle = list_replace_at(&list, &str, size / 2);

  LOG(stderr, "attempting to replace a string at index: %zu\n", size - 1);
  void *last = list_replace_at(&list, &str, size - 1);

  // then
  assert(list_size(&list) == size);
  assert(first && middle && last);
  assert(cmpr(first, strings) == 0);
  assert(cmpr(middle, strings + size / 2) == 0);
  assert(cmpr(last, strings + size - 1) == 0);

  assert(cmpr(list_peek_first(&list), &str) == 0);
  assert(cmpr(list_peek_last(&list), &str) == 0);
  assert(cmpr(list_at(&list, size / 2), &str) == 0);

  // cleanup
  ascii_str_destroy(first);
  ascii_str_destroy(middle);
  ascii_str_destroy(last);

  free(first);
  free(last);
  free(middle);

  after(&list);
}

static void list_index_of_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);

  // when
  LOG(stderr, "attempting to get the index of %s\n", ascii_str_c_str(strings));
  intmax_t first_idx = list_index_of(&list, strings, cmpr);

  LOG(stderr, "attempting to get the index of %s\n", ascii_str_c_str(strings + size / 2));
  intmax_t mid_idx = list_index_of(&list, strings + size / 2, cmpr);

  LOG(stderr, "attempting to get the index of %s\n", ascii_str_c_str(strings + size - 1));
  intmax_t last_idx = list_index_of(&list, strings + size - 1, cmpr);

  // then
  assert(first_idx == 0);
  assert(mid_idx == (intmax_t)size / 2);
  assert(last_idx == (intmax_t)size - 1);

  // cleanup
  after(&list);
}

static void list_iterator_empty_list_test(void) {
  // given
  struct list list = list_create(sizeof(struct ascii_str), destroy_string);

  // when
  void *iter_begin = list_iter_begin(&list);
  void *iter_end = list_iter_end(&list);

  // then
  assert(iter_begin == iter_end);

  // cleanup
  after(&list);
}

static void list_iterator_test(struct ascii_str *strings, size_t size) {
  // given
  struct list list = before(strings, size);

  // when
  void *iter_begin = list_iter_begin(&list);
  void *iter_end = list_iter_end(&list);

  // then
  assert(iter_begin != iter_end);
  assert(list_size(&list) == size);

  for (size_t i = 0; i < size && iter_begin != iter_end; i++) {
    LOG(stderr,
        "strings[%zu]: %s == iter: %s?\n",
        i,
        ascii_str_c_str(strings + i),
        ascii_str_c_str(*(struct ascii_str **)iter_begin));
    assert(cmpr(*(struct ascii_str **)iter_begin, strings + i) == 0);
    iter_begin = list_iter_next(&list, iter_begin);
  }

  // cleanup
  after(&list);
}

int main(void) {
  enum local_sizes {
    SMALL = 2,
    LARGE = 100,
  };

  struct ascii_str strings_large[LARGE];
  struct ascii_str strings_small[SMALL];
  list_prepend_test(strings_large, LARGE);
  list_prepend_test(strings_small, SMALL);

  list_append_test(strings_large, LARGE);
  list_append_test(strings_small, SMALL);

  list_insert_at_test(strings_large, LARGE);
  list_insert_at_test(strings_small, SMALL);

  list_peek_first_empty_list_test();

  list_peek_first_test(strings_large, LARGE);
  list_peek_first_test(strings_small, SMALL);

  list_peek_last_empty_list_test();

  list_peek_last_test(strings_large, LARGE);
  list_peek_last_test(strings_small, SMALL);

  list_at_empty_list_test();

  list_at_test(strings_large, LARGE);
  list_at_test(strings_small, SMALL);

  list_remove_first_empty_list_test();

  list_remove_first_test(strings_large, LARGE);
  list_remove_first_test(strings_small, SMALL);
  list_remove_first_test(strings_small, 1);

  list_remove_last_empty_list_test();

  list_remove_last_test(strings_large, LARGE);
  list_remove_last_test(strings_small, SMALL);

  list_remove_at_rand_empty_list_test();
  list_remove_at_zero_empty_list_test();

  list_remove_at_rand_test(strings_large, LARGE);
  list_remove_at_rand_test(strings_small, SMALL);

  list_remove_at_zero_test(strings_large, LARGE);
  list_remove_at_zero_test(strings_small, SMALL);

  list_remove_at_size_test(strings_large, LARGE);
  list_remove_at_size_test(strings_small, SMALL);

  list_replace_at_empty_list_test();

  list_replace_at_test(strings_large, LARGE);

  list_index_of_test(strings_large, LARGE);
  list_index_of_test(strings_small, SMALL);

  list_iterator_empty_list_test();

  list_iterator_test(strings_large, LARGE);
  list_iterator_test(strings_small, SMALL);
}

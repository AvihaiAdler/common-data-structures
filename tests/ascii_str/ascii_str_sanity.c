#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "include/ascii_str.h"

#define MIN_SSO_LEN (size_t)12 - 1

static void ascii_str_init_null_test(void) {
  // given
  // when
  struct ascii_str empty = ascii_str_init(NULL);

  // then
  assert(empty.is_sso);
  assert(ascii_str_len(&empty) == 0);

  // cleanup
  ascii_str_destroy(&empty);
}

static void ascii_str_init_empty_test(void) {
  // given
  // when
  struct ascii_str empty = ascii_str_init("");

  // then
  assert(empty.is_sso);
  assert(ascii_str_len(&empty) == 0);

  // cleanup
  ascii_str_destroy(&empty);
}

static void ascii_str_init_sso_test(const char *c_str) {
  // given
  assert(c_str);
  size_t len = strlen(c_str);
  assert(len <= MIN_SSO_LEN);

  // when
  struct ascii_str str = ascii_str_init(c_str);

  // then
  assert(str.is_sso);
  assert(ascii_str_len(&str) == len);
  assert(strcmp(ascii_str_c_str(&str), c_str) == 0);

  // cleanup
  ascii_str_destroy(&str);
}

static void ascii_str_init_non_sso_test(const char *c_str) {
  // given
  assert(c_str);
  size_t len = strlen(c_str);
  assert(len > MIN_SSO_LEN);

  // when
  struct ascii_str str = ascii_str_init(c_str);

  // then
  assert(!str.is_sso);
  assert(ascii_str_len(&str) == len);
  assert(strcmp(ascii_str_c_str(&str), c_str) == 0);

  // cleanup
  ascii_str_destroy(&str);
}

static void ascii_str_empty_test(void) {
  // given
  // when
  struct ascii_str empty = ascii_str_init("");

  // then
  assert(empty.is_sso);
  assert(ascii_str_len(&empty) == 0);
  assert(ascii_str_empty(&empty));

  // cleanup
  ascii_str_destroy(&empty);
}

static void ascii_str_non_empty_test(const char *c_str) {
  // given
  assert(c_str);
  size_t len = strlen(c_str);
  assert(len);

  // when
  struct ascii_str empty = ascii_str_init(c_str);

  // then
  assert(!ascii_str_empty(&empty));
  assert(ascii_str_len(&empty) == len);

  // cleanup
  ascii_str_destroy(&empty);
}

static void ascii_str_empty_after_clear_test(const char *c_str) {
  // given
  assert(c_str);
  size_t len = strlen(c_str);
  assert(len);
  struct ascii_str str = ascii_str_init(c_str);
  assert(!ascii_str_empty(&str));

  // when
  ascii_str_clear(&str);

  // then
  assert(ascii_str_empty(&str));

  // cleanup
  ascii_str_destroy(&str);
}

static void ascii_str_push_test(const char *c_str) {
  assert(c_str);
  size_t len = strlen(c_str);
  assert(len);

  // given
  struct ascii_str str = ascii_str_init(c_str);
  assert(!ascii_str_empty(&str));
  char c = 'a';

  // when
  ascii_str_push(&str, c);

  // then
  assert(!ascii_str_empty(&str));
  assert(ascii_str_len(&str) - 1 == len);
  assert(memcmp(ascii_str_c_str(&str), c_str, len));
  assert(ascii_str_pop(&str) == c);

  // cleanup
  ascii_str_destroy(&str);
}

static void ascii_str_push_force_resize_test(const char *c_str) {
  assert(c_str);
  size_t len = strlen(c_str);

  // given
  struct ascii_str str = ascii_str_init(c_str);

  // when
  for (int c = 'a'; c < 'z'; c++) {
    ascii_str_push(&str, c);
  }

  // then
  assert(!str.is_sso);  // max sso size on x64 is 23 chars
  assert(!ascii_str_empty(&str));
  assert(ascii_str_len(&str) == len + 'z' - 'a');

  // cleanup
  ascii_str_destroy(&str);
}

#include "ascii_str.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHORT_BUFFER_SIZE (sizeof(struct ascii_str_long) - 1)
#define GROWTH_FACTOR 1
#define INIT_BUFFER_SIZE (size_t)64

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static char *ascii_str_c_str_internal(struct ascii_str *ascii_str) {
  return ascii_str->is_sso ? ascii_str->_short.data : ascii_str->_long.data;
}

static bool ascii_str_resize_internal(struct ascii_str *ascii_str, size_t requested_size) {
  size_t exp = (size_t)log2(requested_size) + 1;
  size_t capacity = (size_t)pow(2, exp);

  if (capacity <= SHORT_BUFFER_SIZE) return false;

  if (ascii_str->is_sso) {
    char *buf = malloc(capacity);
    if (!buf) return false;

    size_t len = ascii_str_len(ascii_str);
    memcpy(buf, ascii_str->_short.data, len + 1);

    ascii_str->is_sso = false;
    ascii_str->_long = (struct ascii_str_long){.capacity = capacity, .size = len, .data = buf};

    return true;
  }

  char *buf = realloc(ascii_str->_long.data, capacity);
  if (!buf) return false;

  ascii_str->_long = (struct ascii_str_long){.capacity = capacity, .size = ascii_str->_long.size, .data = buf};
  return true;
}

static bool ascii_str_resize(struct ascii_str *ascii_str) {
  return ascii_str->is_sso ? ascii_str_resize_internal(ascii_str, INIT_BUFFER_SIZE - 1) :
                             ascii_str_resize_internal(ascii_str, ascii_str->_long.capacity << 1);
}

static void ascii_str_append_internal(struct ascii_str *restrict ascii_str, char const *restrict arr, size_t len) {
  if (ascii_str->is_sso && (size_t)ascii_str->_short.data[SHORT_BUFFER_SIZE] >= len) {
    size_t ascii_strlen = ascii_str_len(ascii_str);

    // there's enough room in the buffer for c_str
    memcpy(ascii_str->_short.data + ascii_strlen, arr, len);
    ascii_str->_short.data[SHORT_BUFFER_SIZE] -= len;
    return;
  }

  // not enough room in the buffer for c_str
  if ((ascii_str->is_sso && (size_t)ascii_str->_short.data[SHORT_BUFFER_SIZE] < len) ||
      (!ascii_str->is_sso && ascii_str->_long.capacity - ascii_str->_long.size < len + 1)) {
    do { ascii_str_resize(ascii_str); } while (ascii_str->_long.capacity - ascii_str->_long.size < len + 1);
  }

  // copy all of the chars in arr
  memcpy(ascii_str->_long.data + ascii_str->_long.size, arr, len);
  ascii_str->_long.size += len;

  // add a null terminator
  ascii_str->_long.data[ascii_str->_long.size] = 0;
}

static struct ascii_str ascii_str_init_short_internal(void) {
  struct ascii_str str = {.is_sso = true};  // the rest is empty initialized
  str._short.data[SHORT_BUFFER_SIZE] = SHORT_BUFFER_SIZE;
  return str;
}

struct ascii_str ascii_str_create(char const *str, int len) {
  if (!str || !len) { return ascii_str_from_arr(NULL, 0); }

  if (len < STR_C_STR) { return ascii_str_from_arr(NULL, 0); }

  size_t length = len;
  if (len == STR_C_STR) { length = strlen(str); }

  return ascii_str_from_arr(str, length);
}

struct ascii_str ascii_str_from_fmt(char const *fmt, ...) {
  struct ascii_str str = ascii_str_create(NULL, 0);
  if (!fmt) return str;

  va_list args;
  va_start(args, fmt);

  va_list cpy;
  va_copy(cpy, args);
  int len = vsnprintf(NULL, 0, fmt, cpy);
  va_end(cpy);

  if (len < 0) return str;

  if (len > (int)str._short.data[SHORT_BUFFER_SIZE]) {
    if (!ascii_str_resize_internal(&str, (size_t)len + 1)) return str;
  }

  vsnprintf(ascii_str_c_str_internal(&str), len + 1, fmt, args);
  va_end(args);

  if (str.is_sso) {
    str._short.data[SHORT_BUFFER_SIZE] =
      str._short.data[SHORT_BUFFER_SIZE] ? str._short.data[SHORT_BUFFER_SIZE] - len : 0;
  } else {
    str._long.size = len;
  }

  return str;
}

struct ascii_str ascii_str_from_arr(char const *arr, size_t len) {
  struct ascii_str str = ascii_str_init_short_internal();

  if (!arr || !len) return str;

  ascii_str_append_internal(&str, arr, len);
  return str;
}

void ascii_str_destroy(struct ascii_str *ascii_str) {
  if (!ascii_str || ascii_str->is_sso) return;

  if (ascii_str->_long.data) free(ascii_str->_long.data);
}

bool ascii_str_empty(struct ascii_str const *ascii_str) {
  if (!ascii_str) return true;

  if (ascii_str->is_sso) return ascii_str->_short.data[SHORT_BUFFER_SIZE] == SHORT_BUFFER_SIZE;

  return ascii_str->_long.size == 0;
}

size_t ascii_str_len(struct ascii_str const *ascii_str) {
  if (!ascii_str) return 0;

  return ascii_str->is_sso ? SHORT_BUFFER_SIZE - ascii_str->_short.data[SHORT_BUFFER_SIZE] : ascii_str->_long.size;
}

char const *ascii_str_c_str(struct ascii_str *ascii_str) {
  if (!ascii_str) return NULL;

  return ascii_str_c_str_internal(ascii_str);
}

void ascii_str_clear(struct ascii_str *ascii_str) {
  if (!ascii_str) return;

  if (ascii_str->is_sso) {
    ascii_str->_short = (struct ascii_str_short){.data[SHORT_BUFFER_SIZE] = SHORT_BUFFER_SIZE};
    return;
  }

  *(ascii_str->_long.data) = 0;
  ascii_str->_long.size = 0;
}

void ascii_str_push(struct ascii_str *ascii_str, char const c) {
  if (!ascii_str) return;

  if (ascii_str->is_sso) {
    size_t available_chars = ascii_str->_short.data[SHORT_BUFFER_SIZE];
    if (available_chars) {
      ascii_str->_short.data[SHORT_BUFFER_SIZE - available_chars] = c;
      ascii_str->_short.data[SHORT_BUFFER_SIZE]--;
      return;
    }

    // no more available space
    if (!ascii_str_resize(ascii_str)) return;
  } else if (ascii_str->_long.size == ascii_str->_long.capacity - 1) {
    if (!ascii_str_resize(ascii_str)) return;
  }

  ascii_str->_long.data[ascii_str->_long.size] = c;
  ascii_str->_long.data[++ascii_str->_long.size] = 0;
}

char ascii_str_pop(struct ascii_str *ascii_str) {
  if (!ascii_str || ascii_str_empty(ascii_str)) return (char)-1;

  char *buf = ascii_str_c_str_internal(ascii_str);

  char ret = buf[ascii_str_len(ascii_str) - 1];
  buf[ascii_str_len(ascii_str) - 1] = 0;

  if (ascii_str->is_sso) {
    ascii_str->_short.data[SHORT_BUFFER_SIZE]++;
  } else {
    ascii_str->_long.size -= 1;
  }

  return ret;
}

void ascii_str_append(struct ascii_str *restrict ascii_str, char const *restrict c_str) {
  if (!ascii_str || !c_str) return;

  size_t len = strlen(c_str);
  if (!len) return;

  ascii_str_append_internal(ascii_str, c_str, len);
}

void ascii_str_erase(struct ascii_str *ascii_str, size_t from_pos, size_t count) {
  if (!ascii_str || !count) return;

  size_t len = ascii_str_len(ascii_str);
  if (from_pos >= len) return;

  // handle the case where count exceeds the string length
  size_t end_pos = MIN(from_pos + count, len);
  size_t actual_count = end_pos - from_pos;

  char *buf = ascii_str_c_str_internal(ascii_str);
  memmove(buf + from_pos, buf + end_pos, len - end_pos);

  if (ascii_str->is_sso) {
    size_t available_chars = ascii_str->_short.data[SHORT_BUFFER_SIZE] + actual_count;
    // restore the property where all availabe chars are null terminators
    memset(buf + from_pos + len - end_pos, 0, available_chars);

    ascii_str->_short.data[SHORT_BUFFER_SIZE] = available_chars;
  } else {
    ascii_str->_long.data[len - actual_count] = 0;
    ascii_str->_long.size -= actual_count;
  }
}

void ascii_str_insert(struct ascii_str *restrict ascii_str, size_t pos, char const *restrict c_str) {
  if (!ascii_str || !c_str) return;

  if (pos >= ascii_str_len(ascii_str)) {
    ascii_str_append(ascii_str, c_str);
    return;
  }

  size_t len = strlen(c_str);
  size_t capacity = ascii_str->is_sso ? SHORT_BUFFER_SIZE : ascii_str->_long.capacity;

  // check to ensure the current buffer can hold c_str
  size_t available_chars = capacity - ascii_str_len(ascii_str);
  if (len >= available_chars) {
    if (!ascii_str_resize(ascii_str)) return;
  }

  size_t end_pos = pos + len;
  char *buf = ascii_str_c_str_internal(ascii_str);
  memmove(buf + end_pos,
          buf + pos,
          ascii_str_len(ascii_str) - pos);  // shift the right hand side of the slice to the right

  // avoid cases where the user wish to insert the original string in itself (take the performance hit)
  // memmove(buf + pos, c_str, len);
  memcpy(buf + pos, c_str, len);

  if (ascii_str->is_sso) {
    ascii_str->_short.data[SHORT_BUFFER_SIZE] = available_chars - len;
  } else {
    ascii_str->_long.size += len;
    ascii_str->_long.data[ascii_str->_long.size] = 0;
  }
}

bool ascii_str_contains(struct ascii_str *haystack, char const *needle) {
  if (!haystack || !needle) return NULL;

  return strstr(ascii_str_c_str(haystack), needle) != NULL;
}

int ascii_str_index_of(struct ascii_str *ascii_str, char const c) {
  if (!ascii_str) return STR_NEXISTS;

  char const *buf = ascii_str_c_str(ascii_str);
  char const *needle = strchr(buf, c);
  return needle ? (size_t)(needle - buf) : STR_NEXISTS;
}

struct ascii_str ascii_str_substr(struct ascii_str *ascii_str, size_t from_pos, size_t count) {
  if (!ascii_str || !count) return ascii_str_create(NULL, 0);

  if (from_pos >= ascii_str_len(ascii_str)) return ascii_str_create(NULL, 0);

  size_t end_pos = MIN(from_pos + count, ascii_str_len(ascii_str));
  // the string is empty / the requested substring is 0 bytes long
  if (!end_pos) return ascii_str_create(NULL, 0);

  char *slice_start = ascii_str_c_str_internal(ascii_str) + from_pos;
  struct ascii_str ret = ascii_str_from_arr(slice_start, end_pos - from_pos);

  return ret;
}

static inline void ascii_str_destroy_internal(void *ascii_str) {
  ascii_str_destroy(ascii_str);
}

struct vec ascii_str_split(struct ascii_str *restrict ascii_str, char const *restrict pattern) {
  // create a lookup table for all ascii chars
  bool lookup[UCHAR_MAX] = {0};
  for (; *pattern; pattern++) { lookup[(unsigned char)*pattern] = true; }

  struct vec slices = vec_create(sizeof *ascii_str, ascii_str_destroy_internal);

  char const *walker = ascii_str_c_str(ascii_str);
  if (!walker) return slices;

  char const *anchor = walker;
  for (;; walker++) {  // while(true) is iffy here
    if (lookup[(unsigned char)*walker] || *walker == 0) {
      struct ascii_str tmp = ascii_str_from_arr(anchor, (size_t)(walker - anchor));
      (void)vec_push(&slices, &tmp);
      ascii_str_destroy(&tmp);

      anchor = walker + 1;
    }

    if (!*walker) break;
  }

  return slices;
}

void ascii_str_tolower(struct ascii_str *string) {
  if (!string) return;

  for (char *c_str = ascii_str_c_str_internal(string); *c_str; c_str++) { *c_str = tolower((int)*c_str); }
}

void ascii_str_toupper(struct ascii_str *string) {
  if (!string) return;

  for (char *c_str = ascii_str_c_str_internal(string); *c_str; c_str++) { *c_str = toupper((int)*c_str); }
}

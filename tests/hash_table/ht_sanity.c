#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/hash_table.h"

struct string {
  size_t str_size;
  char *str;
};

// strings related functions
int generate_random(int min, int max) {
  int range = max - min + 1;
  double rand_val = rand() / (1.0 + RAND_MAX);
  return (int)(rand_val * range + min);
}

char *generate_random_str(size_t len) {
  char *str = calloc(len + 1, 1);
  assert(str);
  for (size_t i = 0; i < len; i++) {
    str[i] = 'a' + generate_random(0, 25);
  }
  return str;
}

char **generate_keys(size_t num_of_keys) {
  char **keys = calloc(num_of_keys, sizeof *keys);
  assert(keys);

  for (size_t i = 0; i < num_of_keys; i++) {
    size_t len = (size_t)generate_random(10, 20);
    keys[i] = generate_random_str(len);
    assert(keys[i]);
  }
  return keys;
}

void destroy_keys(char **keys, size_t num_of_keys) {
  if (!keys) return;
  for (size_t i = 0; i < num_of_keys; i++) {
    if (keys[i]) free(keys[i]);
  }
  free(keys);
}

struct string generate_random_string(size_t len) {
  char *str = generate_random_str(len);
  return (struct string){.str_size = len, .str = str};
}

struct string *generate_strings(size_t num_of_values) {
  struct string *strings = calloc(num_of_values, sizeof *strings);
  assert(strings);

  for (size_t i = 0; i < num_of_values; i++) {
    size_t len = (size_t)generate_random(10, 20);
    struct string str = generate_random_string(len);
    memcpy(&strings[i], &str, sizeof strings[i]);
  }
  return strings;
}

void destroy_strings(struct string *strings, size_t num_of_strings) {
  if (!strings) return;
  for (size_t i = 0; i < num_of_strings; i++) {
    if (strings[i].str) free(strings[i].str);
  }
  free(strings);
}

int cmpr_keys(const void *key, const void *other) {
  const char *str_key = key;
  const char *str_other = other;
  return strcmp(str_key, str_other);
}

void print_str(char *str, size_t len) {
  for (size_t i = 0; i < len; i++) {
    fputc(str[i], stdout);
  }
  printf("\n");
}

int cmpr_values(const void *val, const void *other) {
  const struct string *str = val;
  const struct string *str_other = other;
  if (str->str_size != str_other->str_size) return -1;
  return strcmp(str->str, str_other->str);
}

void str_destroy(void *str) {
  struct string *s = str;
  if (s && s->str) free(s->str);
}

// unit tests
struct hash_table *before(void) {
  struct hash_table *table = table_init(cmpr_keys, NULL, NULL);
  assert(table);
  assert(table->num_of_elements == 0);
  return table;
}

void after(struct hash_table *table) { table_destroy(table); }

void table_put_empty_table_test(char **keys, size_t keys_size,
                                struct string *strings, size_t strings_size) {
  assert(keys_size == strings_size);  // precondition
  // given
  struct hash_table *table = before();
  assert(table_empty(table));
  assert(table_size(table) == 0);

  // when
  for (size_t i = 0; i < keys_size; i++) {
    struct string *old = table_put(table, keys[i], strlen(keys[i]) + 1,
                                   &strings[i], sizeof strings[i]);
    assert(!old);
  }

  // then
  assert(table_size(table) == keys_size);

  // cleanup
  after(table);
}

void table_put_override_value_test(char **keys, size_t keys_size,
                                   struct string *strings,
                                   size_t strings_size) {
  assert(keys_size == strings_size);  // precondition

  // given
  struct hash_table *table = before();

  for (size_t i = 0; i < keys_size; i++) {
    struct string *old = table_put(table, keys[i], strlen(keys[i]) + 1,
                                   &strings[i], sizeof strings[i]);
    assert(!old);
  }

  char *another_str = "not the original string";
  struct string another_string_same_key = {.str_size = strlen(another_str),
                                           .str = another_str};
  // when
  struct string *old =
      table_put(table, keys[0], strlen(keys[0]), &another_string_same_key,
                sizeof another_string_same_key);
  // then
  assert(old);
  assert(cmpr_values(old, strings) == 0);

  // cleanup
  free(old);
  after(table);
}

void table_put_with_resize_test(size_t size) {
  // precondition
  assert(size > TABLE_INIT_CAPACITY * LOAD_FACTOR);

  // given
  char **keys = generate_keys(size);
  for (size_t i = 0; i < size; i++) {
    print_str(keys[i], strlen(keys[i]));
  }

  struct string *strings = generate_strings(size);

  struct hash_table *table = table_init(cmpr_keys, NULL, str_destroy);

  // when
  for (size_t i = 0; i < size; i++) {
    table_put(table, keys[i], strlen(keys[i]) + 1, &strings[i],
              sizeof strings[i]);
  }

  // then
  assert(table_capacity(table) > TABLE_INIT_CAPACITY);
  for (size_t i = 0; i < size; i++) {
    struct string *str = table_get(table, keys[i], strlen(keys[i]) + 1);
    if (str) assert(cmpr_values(str, &strings[i]) == 0);
  }

  // cleanup
  table_destroy(table);
  free(strings);
  destroy_keys(keys, size);
}

void table_get_test(char **keys, size_t keys_size, struct string *strings,
                    size_t strings_size) {
  // precondition
  assert(keys_size == strings_size);

  // given
  struct hash_table *table = before();

  for (size_t i = 0; i < keys_size; i++) {
    struct string *old = table_put(table, keys[i], strlen(keys[i]) + 1,
                                   &strings[i], sizeof strings[i]);
    assert(!old);
  }

  // when
  struct string *val = table_get(table, keys[0], strlen(keys[0]) + 1);

  // then
  assert(val);
  assert(cmpr_values(val, &strings[0]) == 0);

  // cleanup
  after(table);
}

int main(void) {
  srand(time(NULL));
  size_t num_of_keys = 5;
  size_t num_of_strings = 5;

  char **keys = generate_keys(num_of_keys);
  assert(keys);

  struct string *strings = generate_strings(num_of_strings);
  assert(strings);

  table_put_empty_table_test(keys, num_of_keys, strings, num_of_strings);
  table_put_override_value_test(keys, num_of_keys, strings, num_of_strings);
  table_put_with_resize_test(TABLE_INIT_CAPACITY * LOAD_FACTOR + 1);
  table_get_test(keys, num_of_keys, strings, num_of_strings);

  destroy_keys(keys, num_of_keys);
  destroy_strings(strings, num_of_strings);
}

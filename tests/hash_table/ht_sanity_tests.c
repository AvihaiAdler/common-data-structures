#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "include/hash_table.h"

struct string {
  size_t str_size;
  char *str;
};

int cmpr_keys(const void *key, const void *other) {
  const char *str_key = key;
  const char *str_other = other;
  return strcmp(str_key, str_other);
}

int cmpr_values(const void *val, const void *other) {
  struct string *str = val;
  struct string *str_other = other;
  if (str->str_size != str_other->str_size) return -1;
  return strcmp(str->str, str_other->str);
}

void str_destroy(void *str) {
  struct string *s = str;
  if (s && s->str) free(s->str);
}

struct hash_table *before(void) {
  struct hash_table *table = init_table(cmpr_keys, free, str_destroy);
  assert(table);
  assert(table->num_of_elements == 0);
  return table;
}

void after(struct hash_table *table) { table_destroy(table); }

void table_put_empty_table_test(const char **keys, size_t keys_size,
                                struct string *strings, size_t strings_size) {
  assert(keys_size == strings_size);  // precondition

  // given
  struct hash_table *table = before();

  // when
  for (size_t i = 0; i < keys_size; i++) {
    struct string *old = table_put(table, keys[i], strlen(keys[i]) + 1,
                                   &strings[i], sizeof strings[i]);
    assert(!old);
  }

  // then
  assert(table->num_of_elements == keys_size);

  // cleanup
  after(table);
}
void table_put_override_value_test(const char **keys, size_t keys_size,
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
  char other[] = "not the original string";
  struct string another_string_same_key = {.str_size = strlen(other) + 1,
                                           .str = other};
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

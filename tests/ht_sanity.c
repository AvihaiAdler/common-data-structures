#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ascii_str.h"
#include "hash_table.h"

#define LOG(file, fmt, ...)                                                       \
  do {                                                                            \
    fprintf(file, "%s %s:%d\n\t" fmt, __FILE__, __func__, __LINE__, __VA_ARGS__); \
  } while (0);

static int generate_random(int min, int max) {
  int range = max - min;
  double rand_val = rand() / (1.0 + RAND_MAX);
  return (int)(rand_val * range + min);
}

static size_t hash(void const *key, size_t size) {
  (void)size;

  struct ascii_str *str = (void *)key;
  char const *c_str = ascii_str_c_str(str);

  size_t hash = 5381;
  for (size_t i = 0; i < ascii_str_len(str); i++) {
    hash = hash * 33 + c_str[i];
  }
  return hash;
}

static int cmpr(void const *left, void const *right) {
  struct ascii_str *_left = (void *)left;
  struct ascii_str *_right = (void *)right;
  return strcmp(ascii_str_c_str(_left), ascii_str_c_str(_right));
}

static void destroy_key(void *key) {
  struct ascii_str *_key = key;
  ascii_str_destroy(_key);
}

static struct hash_table before(struct ascii_str *keys, int *values, int *replaced, size_t count) {
  char const *alphabet = "abcdefghijklmnopqrstuvwxyz";
  size_t letters_count = strlen(alphabet);

  struct hash_table table = table_create(sizeof(struct ascii_str), sizeof(int), cmpr, hash, destroy_key, NULL);
  for (size_t i = 0; i < count; i++) {
    int rand_str_len = generate_random(1, 100);
    int rand_value = generate_random(0, INT_MAX);

    struct ascii_str str = ascii_str_create(NULL, 0);
    for (int j = 0; j < rand_str_len; j++) {
      int rand_idx = generate_random(0, letters_count);
      ascii_str_push(&str, alphabet[rand_idx]);
    }

    keys[i] = str;
    values[i] = rand_value;

    int value;
    enum ds_error err = table_put(&table, &str, &rand_value, &value);
    LOG(stderr, "%s : %d\n", ascii_str_c_str(&str), err);
    if (err == DS_VALUE_OK) { replaced[i] = 1; }
  }

  return table;
}

void after(struct hash_table *table, struct ascii_str *keys, int *replaced, size_t count) {
  table_destroy(table);

  for (size_t i = 0; i < count; i++) {
    if (replaced[i]) ascii_str_destroy(keys + i);
  }
}

void table_put_empty_table_test(struct ascii_str key, int value) {
  struct hash_table table = table_create(sizeof(struct ascii_str), sizeof(int), cmpr, hash, destroy_key, NULL);

  int old;
  assert(table_put(&table, &key, &value, &old) == DS_OK);
  assert(!table_empty(&table));
  assert(table_size(&table) == 1);

  table_destroy(&table);
}

void table_put_with_resize_test(void) {
  enum local_size {
    SIZE = 100,
  };

  struct ascii_str keys[SIZE] = {0};
  int values[SIZE] = {0};
  int replaced[SIZE] = {0};

  struct hash_table table = before(keys, values, replaced, SIZE);

  assert(!table_empty(&table));
  assert(table_size(&table) >= SIZE);

  after(&table, keys, replaced, SIZE);
}

void table_remove_test(void) {
  enum local_size {
    SIZE = 100,
  };

  struct ascii_str keys[SIZE] = {0};
  int values[SIZE] = {0};
  int replaced[SIZE] = {0};

  struct hash_table table = before(keys, values, replaced, SIZE);

  struct ascii_str removed = ascii_str_create(ascii_str_c_str(keys), (int)ascii_str_len(keys));

  int old_first;
  enum ds_error err = table_remove(&table, keys, &old_first);
  LOG(stderr, "%s %d\n", ascii_str_c_str(&removed), err);

  assert(err == DS_VALUE_OK);
  assert(old_first == values[0]);

  // key has already been removed
  assert(table_remove(&table, &removed, &old_first) == DS_NOT_FOUND);

  struct ascii_str non_existing_key = ascii_str_create(ascii_str_c_str(keys + SIZE / 2), STR_C_STR);
  ascii_str_push(&non_existing_key, '.');
  assert(table_remove(&table, &non_existing_key, &old_first) == DS_NOT_FOUND);

  after(&table, keys, replaced, SIZE);
  ascii_str_destroy(&non_existing_key);
  ascii_str_destroy(&removed);
}

void table_get_test(void) {
  enum local_size {
    SIZE = 100,
  };

  struct ascii_str keys[SIZE] = {0};
  int values[SIZE] = {0};
  int replaced[SIZE] = {0};

  struct hash_table table = before(keys, values, replaced, SIZE);

  int old_first;
  enum ds_error err = table_get(&table, keys, &old_first);
  LOG(stderr, "%s : %d\n", ascii_str_c_str(keys), err);
  assert(err == DS_VALUE_OK);
  assert(old_first == values[0]);

  struct ascii_str removed = ascii_str_create(ascii_str_c_str(keys), (int)ascii_str_len(keys));

  // key has already been removed
  assert(table_remove(&table, keys, &old_first) == DS_VALUE_OK);
  assert(table_get(&table, &removed, &old_first) == DS_NOT_FOUND);

  struct ascii_str non_existing_key = ascii_str_create(ascii_str_c_str(keys + SIZE / 2), STR_C_STR);
  ascii_str_push(&non_existing_key, '.');
  assert(table_get(&table, &non_existing_key, &old_first) == DS_NOT_FOUND);

  after(&table, keys, replaced, SIZE);
  ascii_str_destroy(&non_existing_key);
  ascii_str_destroy(&removed);
}

int main(void) {
  srand(time(NULL));

  table_put_empty_table_test(ascii_str_create("hello, world", STR_C_STR), 1);
  table_remove_test();
  table_get_test();
}

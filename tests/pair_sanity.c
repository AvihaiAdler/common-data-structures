#include <assert.h>
#include <string.h>
#include "ascii_str.h"
#include "pair.h"

static void pair_create_stack_test(int _key, long _value) {
  struct pair p = pair_create(&_key, sizeof _key, &_value, sizeof _value);

  assert(p.state == PAIR_STACK);

  int key;
  assert(pair_key(&p, &key, sizeof key) == DS_OK);
  assert(_key == key);

  long value;
  assert(pair_value(&p, &value, sizeof value) == DS_OK);
  assert(_value == value);

  pair_destroy(&p);
}

static void pair_create_heap_test(long long _key, struct ascii_str _value) {
  assert(sizeof _value > PAIR_MEMBER_INIT_SIZE);

  struct pair p = pair_create(&_key, sizeof _key, &_value, sizeof _value);
  assert(p.state == PAIR_HEAP);

  long long key;
  assert(pair_key(&p, &key, sizeof key) == DS_OK);
  assert(_key == key);

  struct ascii_str value;
  assert(pair_value(&p, &value, sizeof value) == DS_OK);
  assert(strcmp(ascii_str_c_str(&_value), ascii_str_c_str(&value)) == 0);

  ascii_str_destroy(&value);
  pair_destroy(&p);
}

static void pair_create_ptr_test(int const *_key, char const *_value) {
  struct pair p = pair_create(&_key, sizeof _key, &_value, sizeof _value);
  assert(p.state == PAIR_STACK);

  int *key;
  assert(pair_key(&p, &key, sizeof key) == DS_OK);
  assert(*_key == *key);

  char *value;
  assert(pair_value(&p, &value, sizeof value) == DS_OK);
  assert(strcmp(_value, value) == 0);

  pair_destroy(&p);
}

int main(void) {
  pair_create_stack_test(1, 1U);
  pair_create_heap_test(1ULL, ascii_str_create("short string", STR_C_STR));
  pair_create_heap_test(1ULL, ascii_str_create("very long string to force a heap allocation", STR_C_STR));

  int _key = 1;
  pair_create_ptr_test(&_key, "another_short_string");
  pair_create_ptr_test(&_key, "another_very_long_string_to_force_a_heap_allocation");
}

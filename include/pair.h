#pragma once

#include <stddef.h>
#include "defines.h"

#define PAIR_MEMBER_INIT_SIZE (sizeof(void *))

struct pair_internal_stack {
  unsigned char key[PAIR_MEMBER_INIT_SIZE];
  unsigned char value[PAIR_MEMBER_INIT_SIZE];
};

struct pair_internal_heap {
  void *key;
  void *value;
};

enum pair_state {
  PAIR_EMPTY,
  PAIR_STACK,
  PAIR_HEAP,
};

struct pair {
  union {
    struct pair_internal_stack _ps;
    struct pair_internal_heap _ph;
  };
  size_t key_size;
  size_t value_size;
  enum pair_state state;
};

struct pair pair_create(void const *key, size_t key_size, void const *value, size_t value_size);

void pair_destroy(struct pair *pair);

enum ds_error pair_key(struct pair *restrict pair, void *restrict key, size_t key_size);

enum ds_error pair_value(struct pair *restrict pair, void *restrict value, size_t value_size);

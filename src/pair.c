#include "pair.h"
#include <stdlib.h>
#include <string.h>

struct pair pair_create(void const *key, size_t key_size, void const *value, size_t value_size) {
  if (!key || !value) goto return_empty_pair;

  if (!key_size || !value_size) goto return_empty_pair;

  if (key_size <= PAIR_MEMBER_INIT_SIZE && value_size <= PAIR_MEMBER_INIT_SIZE) {
    struct pair p = {.state = PAIR_STACK, .key_size = key_size, .value_size = value_size};
    (void)memcpy(p._ps.key, key, key_size);
    (void)memcpy(p._ps.value, value, value_size);
    return p;
  }

  struct pair p = {.state = PAIR_HEAP, .key_size = key_size, .value_size = value_size};
  p._ph.key = malloc(key_size);
  if (!p._ph.key) goto return_empty_pair;

  p._ph.value = malloc(value_size);
  if (!p._ph.value) {
    free(p._ph.key);
    goto return_empty_pair;
  }

  (void)memcpy(p._ph.key, key, key_size);
  (void)memcpy(p._ph.value, value, value_size);
  return p;

return_empty_pair:
  return (struct pair){.state = PAIR_EMPTY};
}

void pair_destroy(struct pair *pair) {
  if (!pair) return;

  if (pair->state == PAIR_HEAP) {
    free(pair->_ph.key);
    free(pair->_ph.value);
  }
}

static void *get_key(struct pair *restrict pair) {
  if (!pair) return NULL;

  switch (pair->state) {
    case PAIR_STACK:
      return pair->_ps.key;
    case PAIR_HEAP:
      return pair->_ph.key;
    default:
      return NULL;
  }
}

static void *get_value(struct pair *restrict pair) {
  if (!pair) return NULL;

  switch (pair->state) {
    case PAIR_STACK:
      return pair->_ps.value;
    case PAIR_HEAP:
      return pair->_ph.value;
    default:
      return NULL;
  }
}

enum ds_error pair_key(struct pair *restrict pair, void *restrict key, size_t key_size) {
  if (!pair) return DS_ERROR;
  if (pair->key_size != key_size) return DS_ERROR;

  void *_key = get_key(pair);
  if (!key) return DS_ERROR;

  (void)memcpy(key, _key, key_size);
  return DS_OK;
}

enum ds_error pair_value(struct pair *restrict pair, void *restrict value, size_t value_size) {
  if (!pair) return DS_ERROR;
  if (pair->value_size != value_size) return DS_ERROR;

  void *_value = get_value(pair);
  if (!value) return DS_ERROR;

  (void)memcpy(value, _value, value_size);
  return DS_OK;
}

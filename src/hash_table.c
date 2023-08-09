#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "vec.h"

#define LOAD_FACTOR 0.7
#define TABLE_GROWTH 1
#define TABLE_INIT_CAPACITY 32

/* 'bucket' */
struct kv_pair {
  void *key;
  void *value;

  struct kv_pair *next;
  struct kv_pair *prev;
};

/* entry */
struct entry {
  struct kv_pair *head;
  struct kv_pair *tail;
  struct kv_pair *tmp;  // used internally in the rehash process
};

struct hash_table table_create(size_t key_size,
                               size_t value_size,
                               int (*cmpr)(void const *, void const *),
                               size_t (*hash)(void const *hashable, size_t size),
                               void (*destroy_key)(void *),
                               void (*destroy_value)(void *)) {
  if (!key_size) goto empty_table;
  if (!cmpr) goto empty_table;

  struct vec entries = vec_create(sizeof(struct entry), NULL);
  entries._n_elem = vec_resize(&entries, TABLE_INIT_CAPACITY);

  return (struct hash_table){._cmpr = cmpr,
                             ._destroy_key = destroy_key,
                             ._destroy_value = destroy_value,
                             ._entries = entries,
                             ._hash = hash,
                             ._key_size = key_size,
                             ._n_elem = 0,
                             ._value_size = value_size};

empty_table:
  return (struct hash_table){0};
}

void table_destroy(struct hash_table *table) {
  if (!table || !vec_data(&table->_entries)) return;

  for (size_t i = 0; i < vec_capacity(&table->_entries); i++) {
    // destroy all buckets in an entry
    struct entry *entry = vec_at(&table->_entries, i);
    for (struct kv_pair *bucket = entry->head; bucket; entry->head = bucket) {
      bucket = bucket->next;

      if (table->_destroy_key) { table->_destroy_key(entry->head->key); }
      if (table->_destroy_value) { table->_destroy_value(entry->head->value); }

      if (entry->head->key) free(entry->head->key);
      if (entry->head->value) free(entry->head->value);
      free(entry->head);
    }
  }

  vec_destroy(&table->_entries);
}

bool table_empty(struct hash_table const *table) {
  return table ? table->_n_elem == 0 : true;
}

size_t table_size(struct hash_table const *table) {
  return table ? table->_n_elem : 0;
}

size_t table_capacity(struct hash_table *table) {
  return table ? vec_capacity(&table->_entries) : 0;
}

/* used internally to hash the keys (slightly modified djd2 by Dan Bernstein) */
static inline size_t hash(void const *key, size_t key_size) {
  const unsigned char *k = key;
  size_t hash = 5381;
  for (size_t i = 0; i < key_size; i++, k++) {
    hash = hash * 33 + *k;
  }
  return hash;
}

static inline size_t hash_wrapper(struct hash_table const *table, void const *key) {
  return (table->_hash ? table->_hash(key, table->_key_size) : hash(key, table->_key_size)) %
         vec_capacity(&table->_entries);
}

/* util functions */
/* used internally to create a node and init it with a key-value pair. returns
 * a pointer to a heap allocated node which holds the copies of key and value.
 * both node::key, node::value and node must be free'd. the function assumes
 * key
 * != NULL and key_size > 0 */
static inline struct kv_pair *kv_pair_create(void const *key, size_t key_size, void const *value, size_t value_size) {
  void *_key = malloc(key_size);
  if (!_key) { return NULL; }

  // else - if value_size == 0: kv_pair::value will remain NULL
  void *_value = NULL;
  if (value_size) {
    _value = malloc(value_size);
    if (!_value) {
      free(_key);
      return NULL;
    }
    memcpy(_value, value, value_size);
  }

  memcpy(_key, key, key_size);

  struct kv_pair *kv_pair = malloc(sizeof *kv_pair);
  if (!kv_pair) {
    free(_key);
    free(_value);
    return NULL;
  }
  *kv_pair = (struct kv_pair){.key = _key, .value = _value};

  return kv_pair;
}

/* used internally to replace an existing mapping for a certain key. returns a
 * pointer to the previous key which has to be free'd. the
 * function assumes the node passed in isn't NULL */
static inline void kv_pair_replace_value(struct kv_pair *restrict kv_pair,
                                         void const *restrict value,
                                         size_t value_size,
                                         void *old_value) {
  void *_old_value = kv_pair->value;
  if (_old_value && value_size) { memcpy(old_value, _old_value, value_size); }

  if (value_size) {
    memcpy(kv_pair->value, value, value_size);
  } else {
    kv_pair->value = NULL;
  }
}

/* used internally to prepend a bucket to an entry. retuns true on success,
 * NULL on failure */
static inline bool entry_prepend(struct entry *entry,
                                 void const *key,
                                 size_t key_size,
                                 void const *value,
                                 size_t value_size) {
  struct kv_pair *kv_pair = kv_pair_create(key, key_size, value, value_size);
  if (!kv_pair) return false;

  if (!entry->head) {  // entry is empty
    entry->tail = kv_pair;
  } else {
    entry->head->prev = kv_pair;
  }
  kv_pair->next = entry->head;
  entry->head = kv_pair;
  return true;
}

/* used internally to check whether an entry contains a mapping for a certain
 * key. returns a pointer to the node which contains the same key, or NULL if
 * no such node found */
static inline struct kv_pair *entry_contains(struct entry *entry,
                                             void const *key,
                                             int (*cmpr)(void const *key, void const *other)) {
  if (!entry->head) return NULL;
  for (struct kv_pair *tmp = entry->head; tmp; tmp = tmp->next) {
    if (cmpr(key, tmp->key) == 0) return tmp;
  }
  return NULL;
}

/* used internally to resize (and rehash) the table with minimum
 * allocations/frees */
static inline bool resize_table(struct hash_table *table) {
  if (!table || !vec_data(&table->_entries)) return false;

  size_t old_capacity = vec_capacity(&table->_entries);
  size_t new_capacity = vec_resize(&table->_entries, old_capacity << TABLE_GROWTH);
  if (new_capacity == old_capacity) return false;

  table->_entries._n_elem = new_capacity;

  // rehash every key-value pair
  for (size_t pos = 0; pos < new_capacity; pos++) {
    struct entry *entry = vec_at(&table->_entries, pos);
    if (!entry->head) continue;  // entry is empty

    /* stopping condition. we will iterate over every bucket until we reach
     * entry->tmp */
    entry->tmp = entry->tail;

    // rehash every bucket in a given entry
    for (struct kv_pair *tmp = entry->head; tmp; tmp = tmp->next) {
      // fix node 'relations'
      if (!tmp->next) {  // this bucket is the only bucket in the entry
        entry->head = entry->tail = NULL;
      } else {  // this bucket is entry::head
        entry->head = entry->head->next;
        entry->head->prev = NULL;
      }

      size_t new_pos = hash_wrapper(table, tmp->key);

      struct entry *new_entry = vec_at(&table->_entries, new_pos);

      if (!new_entry->head) {  // new entry is empty
        new_entry->head = new_entry->tail = tmp;
        tmp->prev = NULL;
      } else {
        new_entry->tail->next = tmp;
        tmp->prev = new_entry->tail;
        new_entry->tail = tmp;
      }
      tmp->next = NULL;

      // rehashed all the buckets in the entry
      if (tmp == entry->tmp) {
        entry->tmp = NULL;
        break;
      }
    }
  }
  return true;
}

enum ds_error table_put(struct hash_table *restrict table, void const *key, void const *new_value, void *old_value) {
  if (!table || !vec_data(&table->_entries)) return DS_ERROR;
  if (!key || !old_value) return DS_ERROR;

  // load factor exceeded
  if (vec_capacity(&table->_entries) * LOAD_FACTOR < table->_n_elem + 1) {
    if (!resize_table(table)) return DS_ERROR;
  }

  // get the entry index from the hash
  size_t pos = hash_wrapper(table, key);

  struct entry *entry = vec_at(&table->_entries, pos);
  if (!entry) return DS_OUT_OF_BOUNDS;

  // there's an existing mapping for this key
  struct kv_pair *same_key = entry_contains(entry, key, table->_cmpr);
  if (same_key) {
    kv_pair_replace_value(same_key, new_value, table->_value_size, old_value);
    return DS_VALUE_OK;
  }

  // there isn't an existing mapping for this key
  bool success = entry_prepend(entry, key, table->_key_size, new_value, table->_value_size);
  if (!success) return DS_NO_MEM;

  table->_n_elem++;
  return DS_OK;
}

enum ds_error table_remove(struct hash_table *restrict table, void const *restrict key, void *restrict old_value) {
  if (!table || !vec_data(&table->_entries)) return DS_ERROR;
  if (!key) return DS_ERROR;

  size_t pos = hash_wrapper(table, key);
  struct entry *entry = vec_at(&table->_entries, pos);
  if (!entry) return DS_OUT_OF_BOUNDS;

  struct kv_pair *removed = entry_contains(entry, key, table->_cmpr);
  if (!removed) return DS_NOT_FOUND;  // the table doesn't contains the key `key`

  // the node is the only node in the entry
  if (!removed->next && !removed->prev) {
    entry->head = entry->tail = NULL;
  } else if (!removed->next) {  // removed node is entry::tail
    entry->tail = entry->tail->prev;
    entry->tail->next = NULL;
  } else if (!removed->prev) {  // removed node is entry::head
    entry->head = entry->head->next;
    entry->head->prev = NULL;
  } else {
    removed->prev->next = removed->next;
    removed->next->prev = removed->prev;
  }

  enum ds_error ret = DS_OK;
  if (!old_value && table->_destroy_value) {
    table->_destroy_value(removed->value);
  } else if (old_value) {
    memcpy(old_value, removed->value, table->_value_size);
    ret = DS_VALUE_OK;
  }

  if (table->_destroy_key) { table->_destroy_key(removed->key); }
  free(removed->key);
  free(removed->value);
  free(removed);
  table->_n_elem--;

  return ret;
}

enum ds_error table_get(struct hash_table *restrict table, void const *restrict key, void *restrict value) {
  if (!table || !vec_data(&table->_entries)) return DS_ERROR;
  if (!key || !value) return DS_ERROR;

  size_t pos = hash_wrapper(table, key);
  struct entry *entry = vec_at(&table->_entries, pos);
  if (!entry) return DS_OUT_OF_BOUNDS;

  struct kv_pair *looked_for = entry_contains(entry, key, table->_cmpr);
  if (!looked_for) return DS_NOT_FOUND;

  memcpy(value, looked_for->value, table->_value_size);
  return DS_VALUE_OK;
}

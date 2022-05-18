#include "include/hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "include/vector.h"

struct hash_table *init_table(int (*cmpr)(const void *key, const void *other),
                              void (*destroy_key)(void *key),
                              void (*destroy_value)(void *value)) {
  if (!cmpr) return NULL;

  struct hash_table *table = calloc(1, sizeof *table);
  if (!table) return NULL;

  table->entries = vector_init(sizeof *table->entries);
  if (!table->entries) {
    free(table);
    return NULL;
  }

  table->num_of_elements = 0;
  table->cmpr = cmpr;
  table->destroy_key = destroy_key;
  table->destroy_value = destroy_value;

  // table::capacity is at least INIT_CAPACITY (might be higher if vector init
  // capacity > INIT_CAPACITY)
  table->capacity = vector_reserve(table->entries, INIT_CAPACITY);
  return table;
}

void table_destroy(struct hash_table *table) {
  if (!table) return;
  if (!table->entries) return;

  for (unsigned long long i = 0; i < table->capacity; i++) {
    // destroy all buckets in an entry
    struct entry *entry = vector_at(table->entries, i);
    for (struct node *bucket = entry->head; bucket; entry->head = bucket) {
      bucket = bucket->next;

      if (table->destroy_key) {
        table->destroy_key(bucket->key);
      }

      if (table->destroy_value) {
        table->destroy_value(bucket->value);
      }

      if (bucket->key) free(bucket->key);
      if (bucket->value) free(bucket->value);
      free(entry->head);
    }
  }
  vector_destroy(table->entries, NULL);
  free(table);
}

bool table_empty(struct hash_table *table) {
  if (!table) return false;
  return table->num_of_elements == 0;
}

unsigned long long table_size(struct hash_table *table) {
  if (!table) return 0;
  return table->num_of_elements;
}

/* used internally to hash the keys (slightly modified djd2 by Dan Bernstein)
 */
static unsigned long long hash(const void *key, unsigned long long key_size) {
  const unsigned char *k = key;
  unsigned long long hash = 5381;
  for (unsigned long long i = 0; i < key_size; i++, k++) {
    hash = hash * 33 + *k;
  }
  return hash;
}

/* util functions */
/* used internally to create a node and init it with a key-value pair. returns
 * a pointer to a heap allocated node which holds the copies of key and value.
 * both node::key, node::value and node must be free'd. the function assumes
 * key
 * != NULL and key_size > 0 */
static struct node *init_node(const void *key, unsigned long long key_size,
                              const void *value,
                              unsigned long long value_size) {
  struct node *node = calloc(1, sizeof *node);
  if (!node) return NULL;

  node->key = calloc(key_size, 1);
  if (!node->key) {
    free(node);
    return NULL;
  }

  // else - if value_size == 0: node::value will remain NULL
  if (value_size) {
    node->value = calloc(value_size, 1);
    if (!node->value) {
      free(node->key);
      free(node);
      return NULL;
    }
    memcpy(node->value, value, value_size);
  }

  node->key_size = key_size;
  node->value_size = value_size;

  memcpy(node->key, key, node->key_size);
  return node;
}

/* used internally to replace an existing mapping for a certain key. returns a
 * pointer to the previous key which has to be free'd. the
 * function assumes the node passed in isn't NULL */
static void *node_replace_value(struct node *node, const void *value,
                                unsigned long long value_size) {
  void *old_value = node->value;
  if (value_size) {
    unsigned char *tmp_value = calloc(value_size, 1);
    if (!tmp_value) return NULL;

    memcpy(tmp_value, value, value_size);
    node->value = tmp_value;
  } else {
    node->value = NULL;
  }
  node->value_size = value_size;
  return old_value;
}

/* used internally to prepend a bucket to an entry. retuns true on success,
 * NULL on failure */
static bool entry_prepend(struct entry *entry, const void *key,
                          unsigned long long key_size, const void *value,
                          unsigned long long value_size) {
  struct node *node = init_node(key, key_size, value, value_size);
  if (!node) return false;

  if (entry->head) {
    entry->head->prev = node->next;
  } else {
    entry->tail = node;
  }
  node->next = entry->head;
  entry->head = node;
  return true;
}

/* used internally to check whether an entry contains a mapping for a certain
 * key. returns a pointer to the node which contains the same key, or NULL if
 * no such node found */
static struct node *entry_contains(struct entry *entry, const void *key,
                                   int (*cmpr)(const void *key,
                                               const void *other)) {
  if (!entry->head) return NULL;
  for (struct node *tmp = entry->head; tmp; tmp = tmp->next) {
    if (cmpr(key, tmp->key) == 0) return tmp;
  }
  return NULL;
}

/* used internally to resize (and rehash) the table with minimum
 * allocations/frees */
static bool resize_table(struct hash_table *table) {
  if (!table) return false;
  if (!table->entries) return false;

  unsigned long long new_capacity =
      vector_reserve(table->entries, table->capacity << TABLE_GROWTH);
  if (new_capacity == table->capacity) return false;

  table->capacity = new_capacity;

  // rehash every key-value pair
  for (unsigned long long pos = 0; pos < table->capacity; pos++) {
    struct entry *entry = vector_at(table->entries, pos);
    if (!entry->head) continue;  // entry is empty

    /* stopping condition. we will iterate over every bucket until we reach
     * entry->tmp */
    entry->tmp = entry->tail;

    // rehash every bucket in a given entry
    for (struct node *tmp = entry->head; tmp; tmp = tmp->next) {
      // fix node 'relations'
      if (!tmp->next) {  // this bucket is the only bucket in the entry
        entry->head = entry->tail = NULL;
      } else {  // this bucket is entry::head
        entry->head = entry->head->next;
        entry->head->prev = NULL;
      }

      unsigned long long new_pos =
          hash(tmp->key, tmp->key_size) % table->capacity;
      struct entry *new_entry = vector_at(table->entries, new_pos);

      if (!new_entry->head) {  // new entry is empty
        new_entry->head = new_entry->tail = tmp;
      } else {
        new_entry->tail->next = tmp;
        tmp->prev = new_entry;
        tmp->next = NULL;
        new_entry->tail = tmp;
      }

      // rehashed all the buckets in the entry
      if (tmp == entry->tmp) {
        entry->tmp = NULL;
        break;
      }
    }
  }
  return true;
}

void *table_put(struct hash_table *table, const void *key,
                unsigned long long key_size, const void *value,
                unsigned long long value_size) {
  if (!table) return NULL;
  if (!table->entries) return NULL;
  if (!key && !key_size) return NULL;

  // load factor exceeded
  if (table->capacity * LOAD_FACTOR < table->num_of_elements + 1) {
    if (!resize_table(table)) return NULL;
  }

  // get the entry index from the hash
  unsigned long long pos = hash(key, key_size) % table->capacity;
  struct entry *entry = vector_at(table->entries, pos);
  if (!entry) return NULL;

  // there's an existing mapping for this key
  struct node *contains_same_key = entry_contains(entry, key, table->cmpr);
  if (contains_same_key) {
    return node_replace_value(contains_same_key, value, value_size);
  }

  // there isn't an existing mapping for this key
  bool success = entry_prepend(entry, key, key_size, value, value_size);
  if (!success) return NULL;

  table->num_of_elements++;
  return NULL;
}

void *table_remove(struct hash_table *table, const void *key,
                   unsigned long long key_size) {
  if (!table) return NULL;
  if (!table->entries) return NULL;
  if (!key && !key_size) return NULL;

  unsigned long long pos = hash(key, key_size) % table->capacity;
  struct entry *entry = vector_at(table->entries, pos);
  if (!entry) return NULL;

  struct node *removed = entry_contains(entry, key, table->cmpr);
  if (!removed) return NULL;  // the table doesn't contains the key key

  // the node is the only node in the entry
  if (entry->head == entry->tail) {
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

  void *old_value = removed->value;
  if (table->destroy_key) {
    table->destroy_key(removed->key);
  }
  free(removed->key);
  free(removed);

  return old_value;
}

void *table_get(struct hash_table *table, const void *key,
                unsigned long long key_size) {
  if (!table) return NULL;
  if (!table->entries) return NULL;
  if (!key && !key_size) return NULL;

  unsigned long long pos = hash(key, key_size) % table->capacity;
  struct entry *entry = vector_at(table->entries, pos);
  if (!entry) return NULL;

  struct node *looked_for = entry_contains(entry, key, table->cmpr);
  if (!looked_for) return NULL;

  return looked_for->value;
}
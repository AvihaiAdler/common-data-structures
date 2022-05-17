#include "include/hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "include/vector.h"

struct hash_table *init_table(unsigned long long key_size,
                              unsigned long long value_size) {
  struct hash_table *table = calloc(1, sizeof *table);
  if (!table) return NULL;

  table->entries = vector_init(sizeof(struct entry));
  if (!table->entries) {
    free(table);
    return NULL;
  }

  table->num_of_entries = table->num_of_elements = 0;

  // table::capacity is at least INIT_CAPACITY (might be higher if vector init
  // capacity > INIT_CAPACITY)
  table->capacity = vector_reserve(table->entries, INIT_CAPACITY);
  return table;
}

void table_destroy(struct hash_table *table,
                   void (*destroy)(void *key, void *value)) {
  if (!table) return;
  if (!table->entries) return;

  for (unsigned long long i = 0; i < table->num_of_entries; i++) {
    // destroy all buckets in an entry
    struct entry *entry = vector_at(table->entries, i);
    for (struct node *bucket = entry->head; bucket; entry->head = bucket) {
      bucket = bucket->next;

      if (destroy) {
        destroy(entry->head->key, entry->head->prev);
      }

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

/* used internally to hash the keys (slightly modified djd2 by Dan Bernstein).
 * can be improved by operating on more than one byte at a time */
static unsigned long long hash(const void *key, unsigned long long key_size) {
  const unsigned char *k = key;
  unsigned long long hash = 5381;
  for (unsigned long long i = 0; i < key_size; i++, k++) {
    hash = hash * 33 + *k;
  }
  return hash;
}

/* util functions */
/* used internally to create a node and init it with a key-value pair. returns a
 * pointer to a heap allocated node which holds the copied of key and value.
 * both node::key, node::value and node must be free'd */
static struct node *node_init(const void *key, unsigned long long key_size,
                              const void *value,
                              unsigned long long value_size) {
  struct node *node = calloc(1, sizeof *node);
  if (!node) return NULL;

  node->key = calloc(key_size, 1);
  if (!node->key) {
    free(node);
    return NULL;
  }

  node->value = calloc(value_size, 1);
  if (!node->value) {
    free(node->key);
    free(node);
    return NULL;
  }

  node->key_size = key_size;
  node->value_size = value_size;

  memcpy(node->key, key, node->key_size);
  memcpy(node->value, value, value_size);
  return node;
}

/* used internally to replace an existing mapping for a certain key. returns a
 * copy of the previous key which has to be free'd */
static void *entry_replace(struct entry *entry, const void *key,
                           unsigned long long key_size, const void *value,
                           unsigned long long value_size) {}

/* used internally to prepend a bucket to an entry. retuns true on success, NULL
 * on failure */
static bool entry_prepend(struct entry *entry, const void *key,
                          unsigned long long key_size, const void *value,
                          unsigned long long value_size) {}

/* used internally to check whether an entry contains a mapping for a certain
 * key */
static bool entry_contains(struct entry *entries, const void *key,
                           unsigned long long key_size) {}

/* used internally to resize (and rehash) the table */
static bool resize_table(struct table *table) {}

/* used internally to get the number of buckets in an entry */
unsigned long long entry_size(struct entry *entry) {
  if (!entry) return 0;
  return entry->size;
}

void *table_put(struct hash_table *table, const void *key,
                unsigned long long key_size, const void *value,
                unsigned long long value_size) {
  if (!table) return NULL;
  if (!table->entries) return NULL;

  // load factor exceeded
  if (table->num_of_elements / (double)table->num_of_elements > LOAD_FACTOR) {
    if (!resize_table(table)) return NULL;
  }

  // get the entry index from the hash
  unsigned long long pos = hash(key, key_size) % table->capacity;
  struct entry *entry = vector_at(table->entries, pos);

  // there's an existing mapping for this key
  if (entry_contains(entry, key, key_size)) {
    void *old_value = entry_replace(entry, key, key_size, value, value_size);
    if (!old_value) return NULL;

    table->num_of_elements++;
    return old_value;
  }

  // there isn't an existing mapping for this key
  bool success = entry_prepend(entry, key, key_size, value, value_size);
  if (!success) return NULL;

  if (entry_size(entry) == 1) {
    table->num_of_entries++;
  }
  table->num_of_elements++;
  return NULL;
}
#include "include/hash_table.h"

#include <stdlib.h>

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

  table->key_size = key_size;
  table->value_size = value_size;
  table->num_of_elements = 0;
  table->num_of_entries = vector_reserve(table->entries, INIT_CAPACITY);
  return table;
}

void table_destroy(struct hash_table *table,
                   void (*destroy)(void *key, void *value)) {
  if (!table) return;
  if (!table->entries) return;

  for (unsigned long long i = 0; i < table->num_of_entries; i++) {
    // destroy all buckets
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

unsigned long long hash(const void *key, unsigned long long key_size) {
  const unsigned char *k = key;
  unsigned long long hash = 5381;
  for (unsigned long long i = 0; i < key_size; i++, k++) {
    hash = hash * 33 + *k;
  }
  return hash;
}
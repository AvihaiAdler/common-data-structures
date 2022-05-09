#ifndef HASH_MAP_H_
#define HASH_MAP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define TABLE_GROWTH 2

#define GENERATE_HASH_TABLE(KEY_TYPE, VALUE_TYPE, NAME)                     \
  struct NAME##_bucket {                                                    \
    KEY_TYPE key;                                                           \
    VALUE_TYPE value;                                                       \
    struct NAME##_bucket *next;                                             \
  };                                                                        \
                                                                            \
  struct NAME##_hash_map {                                                  \
    size_t capacity;                                                        \
    size_t num_of_entries;                                                  \
    size_t num_of_buckets;                                                  \
    struct NAME##_bucket **entries;                                         \
  };                                                                        \
                                                                            \
  /* (slightly modified) Dan Bernstain's hash function*/                    \
  uint64_t NAME##_hash(const void *key, size_t key_size) {                  \
    const uint8_t *k = key;                                                 \
    uint64_t hash = 5381;                                                   \
    for (size_t i = 0; i < key_size; i++, k++) {                            \
      hash = hash * 33 + *k;                                                \
    }                                                                       \
    return hash;                                                            \
  }                                                                         \
                                                                            \
  /* resize the table when the load_factor is equal to or above 0.8 */      \
  void NAME##_rehash(struct NAME##_hash_map *map) {                         \
    double load_factor = map->num_of_entries / (double)map->num_of_buckets; \
    if (load_factor < 0.8) return;                                          \
                                                                            \
    /* rehash the entire table */                                           \
    size_t new_capacity = map->capacity * TABLE_GROWTH;                     \
    struct NAME##_bucket **new_entries =                                    \
        calloc(new_capacity, sizeof *new_entries);                          \
    if (!new_entries) return;                                               \
                                                                            \
    size_t new_entries_num = 0;                                             \
    size_t new_buckets_num = 0;                                             \
                                                                            \
    /* go over each entry of the map and rehash every bucket */             \
    for (size_t i = 0; i < map->num_of_entries; i++) {                      \
      struct NAME##_bucket *bucket = map->entries[i];                       \
      if (bucket) {                                                         \
        for (; bucket; bucket = bucket->next) {                             \
          /*all underlying data has to be of type ptr?*/                    \
          uint64_t pos = NAME##_hash(bucket->key, sizeof bucket->key) %     \
                         map->new_capacity;                                 \
          if (!new_entries[pos]) {                                          \
            new_entries_num++;                                              \
          }                                                                 \
          new_buckets_num++;                                                \
          NAME##_bucket_prepend(&new_entries[pos], bucket->key,             \
                                bucket->value);                             \
        }                                                                   \
      }                                                                     \
    }                                                                       \
    NAME##_destroy_entries(map->entries, map->capacity);                    \
    map->capacity = new_capacity;                                           \
    map->num_of_entries = new_entries_num;                                  \
    map->num_of_buckets = new_buckets_num;                                  \
    map->entries = new_entries;                                             \
  }

#endif
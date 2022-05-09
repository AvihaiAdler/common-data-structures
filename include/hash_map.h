#ifndef HASH_MAP_H_
#define HASH_MAP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "key_type.h"

#define TABLE_GROWTH 2

#define GENERATE_HASH_TABLE(KEY_TYPE, VALUE_TYPE, NAME)                        \
  struct NAME##_bucket {                                                       \
    KEY_TYPE key;                                                              \
    VALUE_TYPE value;                                                          \
    enum type key_type;                                                        \
    size_t key_size;                                                           \
    struct NAME##_bucket *next;                                                \
  };                                                                           \
                                                                               \
  struct NAME##_hash_map {                                                     \
    size_t capacity;                                                           \
    size_t num_of_entries;                                                     \
    size_t num_of_buckets;                                                     \
    struct NAME##_bucket **entries;                                            \
  };                                                                           \
                                                                               \
  /* destroys map entries. internal use only */                                \
  void NAME##_map_destroy_entries(struct NAME##_bucket **entries,              \
                                  size_t num_of_entries,                       \
                                  void (*destroy_key)(KEY_TYPE key),           \
                                  void (*destroy_value)(VALUE_TYPE value)) {   \
    if (!entries) return;                                                      \
    for (size_t i = 0; i < num_of_entries; i++) {                              \
      for (struct NAME##_bucket *bucket = entries[i]; entries[i];              \
           entries[i] = bucket) {                                              \
        bucket = bucket->next;                                                 \
        if (destroy_key) destroy_key(entries[i]->key);                         \
        if (destroy_value) destroy_value(entries[i]->value);                   \
        free(entries[i]);                                                      \
      }                                                                        \
    }                                                                          \
    free(entries);                                                             \
  }                                                                            \
                                                                               \
  void NAME##_map_destroy(struct NAME##_hash_map *map,                         \
                          void (*destroy_key)(KEY_TYPE key),                   \
                          void (*destroy_value)(VALUE_TYPE value)) {           \
    if (!map) return;                                                          \
    NAME##_map_destroy_entries(map->entries, map->num_of_entries, destroy_key, \
                               destroy_value);                                 \
    free(map);                                                                 \
  }                                                                            \
                                                                               \
  /* (slightly modified) Dan Bernstain's hash function*/                       \
  uint64_t NAME##_key_hash(const void *key, size_t key_size) {                 \
    const uint8_t *k = key;                                                    \
    uint64_t hash = 5381;                                                      \
    for (size_t i = 0; i < key_size; i++, k++) {                               \
      hash = hash * 33 + *k;                                                   \
    }                                                                          \
    return hash;                                                               \
  }                                                                            \
                                                                               \
  /* resize the table when the load_factor is equal to or above 0.8 */         \
  void NAME##_map_rehash(struct NAME##_hash_map *map) {                        \
    double load_factor = map->num_of_entries / (double)map->num_of_buckets;    \
    if (load_factor < 0.8) return;                                             \
                                                                               \
    /* rehash the entire table */                                              \
    size_t new_capacity = map->capacity * TABLE_GROWTH;                        \
    struct NAME##_bucket **new_entries =                                       \
        calloc(new_capacity, sizeof *new_entries);                             \
    if (!new_entries) return;                                                  \
                                                                               \
    size_t new_entries_num = 0;                                                \
    size_t new_buckets_num = 0;                                                \
                                                                               \
    /* go over each entry of the map and rehash every bucket */                \
    for (size_t i = 0; i < map->num_of_entries; i++) {                         \
      struct NAME##_bucket *bucket = map->entries[i];                          \
      if (bucket) {                                                            \
        for (; bucket; bucket = bucket->next) {                                \
          uint64_t pos;                                                        \
          switch (bucket->type) {                                              \
            case PTR:                                                          \
              pos = NAME##_key_hash(bucket->key, bucket->key_size) %           \
                    new_capacity;                                              \
              break;                                                           \
            case RAW:                                                          \
              pos = NAME##_key_hash(&bucket->key, bucket->key_size) %          \
                    new_capacity;                                              \
              break;                                                           \
          }                                                                    \
                                                                               \
          if (!new_entries[pos]) {                                             \
            new_entries_num++;                                                 \
          }                                                                    \
          new_buckets_num++;                                                   \
          struct NAME##_bucket *tmp = NAME##_copy_bucket(bucket);              \
          NAME##_bucket_prepend(&new_entries[pos], tmp);                       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    NAME##_map_destroy_entries(map->entries, map->num_of_entries, NULL, NULL); \
    map->capacity = new_capacity;                                              \
    map->num_of_entries = new_entries_num;                                     \
    map->num_of_buckets = new_buckets_num;                                     \
    map->entries = new_entries;                                                \
  }

#endif
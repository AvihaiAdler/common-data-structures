#pragma once

#include <stddef.h>

#include "defines.h"
#include "vec.h"

/**
 * @file hash_table.h
 * @brief the definition of a hash table
 *
 * important note:
 * the table stores _copies_ of whatever is passed into it. this means that storing pointers in it must
 * be accounted for when writing the destructor / hash function for said ptr. e.g. if a map looks like `map<int, int *>`
 * - the destructor for the value _must_ cast the ptr it gets to an `int **`. i.e.
 * ```
 * void destroy_value(void *value) {
 *  int **_value = value;
 *  free(*_value);
 * }```
 *
 * storing a ptr into the table means the table take ownership of said ptr
 */

struct hash_table {
  size_t _n_elem;
  size_t _key_size;
  size_t _value_size;

  struct vec _entries;

  int (*_cmpr)(void const *key, void const *other);
  size_t (*_hash)(void const *key, size_t size);
  void (*_destroy_key)(void *key);
  void (*_destroy_value)(void *value);
};

/**
 * @brief creates a hash table object `map<K, V>`
 *
 * @param[in] key_size  the size of every `key` in bytes
 * @param[in] value_size  the size of every `value` in bytes
 * @param[in] cmpr  a function comparing `2` keys. the function must return a positive integer if the `left key > right
 * key`, negative integer if `left key < right key` or `0` if `left key == right key`
 * @param[in, optional] hash - a function generating a hash from a key. the function may be `NULL` in which case
 * `http://www.cse.yorku.ca/~oz/hash.html` will be used
 * @param[in, optional] destroy_key a destructor for `key`
 * @param[in, optional] destroy_value a destructor for `value`
 * @return `struct hash_table` hash table object
 */
struct hash_table table_create(size_t key_size,
                               size_t value_size,
                               int (*cmpr)(void const *, void const *),
                               size_t (*hash)(void const *hashable, size_t size),
                               void (*destroy_key)(void *),
                               void (*destroy_value)(void *));

/**
 * @brief destroys a table
 *
 * @param[in] table the table to destroy. if the table was supplied destructors for its `key` / `value` - the function
 * will call them for each `key` / `value` pair
 */
void table_destroy(struct hash_table *table);

/**
 * @brief returns the state of the table
 *
 * @param[in] table
 * @return `true` if there's no elements in the table
 * @return `false` if there's at least one element in the table
 */
bool table_empty(struct hash_table const *table);

/**
 * @brief returns the number of elements in the table
 *
 * @param[in] table
 * @return `size_t` the number of elements the table contains
 */
size_t table_size(struct hash_table const *table);

/**
 * @brief returns the number of entries in the table
 *
 * @param table
 * @return `size_t` the number of entries in the table
 */
size_t table_capacity(struct hash_table const *table);

/**
 * @brief inserts `new_value` into the table and returns the `old_value` associated with `key` if there was any
 *
 * @param[in] table
 * @param[in] key the mapping for `new_value`
 * @param[in] new_value the value to insert into the table
 * @param[out, optional] old_value a pointer to the type of `value`. the old value will be copied into it
 * @return `enum ds_error` - `DS_OK` if the operation succeded without replacing any old values. `DS_VALUE_OK` if the
 * operation succeded & an old value was replaced and put into `old_value`. `DS_ERROR` otherwise
 */
enum ds_error table_put(struct hash_table *restrict table,
                        void const *key,
                        void const *new_value,
                        void *restrict old_value);

/**
 * @brief removed the mapping for `key` (deletes a `key / value` pair)
 *
 * @param[in] table
 * @param[in] key
 * @param[out, optional] old_value a pointer to the type of `value`. if such pointer isn't `NULL` the old value will be
 * copied into it. if such pointer is `NULL` and the table was assign a destructor for `value` - said destructor will be
 * called on the removed value
 * @return `enum ds_error` - `DS_OK` if the operation succeded. `DS_VALUE_OK` if the operation succeded & the old value
 * was placed into `old_value`. `DS_ERROR` otherwise
 */
enum ds_error table_remove(struct hash_table *restrict table, void const *restrict key, void *restrict old_value);

/**
 * @brief gets the `value` associated with `key`
 *
 * @param[in] table
 * @param[in] key
 * @param[out] value a pointer to the type of `value`. the value will be copied into it
 * @return `enum ds_error` - `DS_VALUE_OK` if the operation succeded. `DS_ERROR` otherwise
 */
enum ds_error table_get(struct hash_table *restrict table, void const *restrict key, void *restrict value);

void print_table(struct hash_table *table, void (*print)(void const *, void const *, size_t));

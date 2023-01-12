#pragma once
#include <stdbool.h>
#include <stddef.h>

struct bst;

/* creates a binary search tree. requires a compare function between 2 keys
 * which returns a positive int if key > other, 0 if key == other or a negative
 * int if key < other.
 *
 * can recieve 2 optional destroy functions one for the key and one for the
 * value. if one of them isn't heap allocated - its corresponding destroy
 * function should be NULL.
 *
 * returns a pointer to a heap allocated binary search tree on success, NULL on
 * failure */
struct bst *bst_create(int (*cmpr)(void *key, void *other),
                       void (*destroy_key)(void *key),
                       void (*destroy_value)(void *value));

/* assosiates a value with a key. if key doesn't exists - creates a new node in
 * the tree with key and value. if key exists - destructs the old value and
 * assosiate the key with the new value.
 *
 * returns true on success, false on failure */
bool bst_upsert(struct bst *bst, const void *const key, size_t key_size,
                const void *const value, size_t value_size);

/* find the value assosiated with a key
 *
 * returns a pointer to said value on success, NULL on failure */
void *bst_find(struct bst *bst, void *key);

/* deletes a key value pair from the tree.
 *
 * returns true on success, NULL on failure */
bool bst_delete(struct bst *bst, void *key);

/* prints the tree (in order traversal)*/
void bst_print(struct bst *bst, void (*print_key)(void *key),
               void (*print_value)(void *value));

/* destroy the tree and all of its data */
void bst_destroy(struct bst *bst);
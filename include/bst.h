#pragma once
#include <stdbool.h>
#include <stddef.h>

struct bst;

struct bst *bst_init(int (*cmpr)(void *key, void *other),
                     void (*destroy_key)(void *key),
                     void (*destroy_value)(void *value));

void bst_destroy(struct bst *tree);

bool bst_upsert(struct bst *tree, const void *const key, size_t key_size,
                const void *const value, size_t value_size);

void *bst_find(struct bst *tree, void *key);

bool bst_delete(struct bst *tree, void *key);

void bst_print(struct bst *tree, void (*print_key)(void *key),
               void (*print_value)(void *value));
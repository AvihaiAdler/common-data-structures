#pragma once
#include <stdbool.h>
#include <stddef.h>

struct bst;

struct bst *bst_create(int (*cmpr)(void *key, void *other),
                       void (*destroy_key)(void *key),
                       void (*destroy_value)(void *value));

bool bst_upsert(struct bst *bst, const void *const key, size_t key_size,
                const void *const value, size_t value_size);

void *bst_find(struct bst *bst, void *key);

bool bst_delete(struct bst *bst, void *key);

void bst_print(struct bst *bst, void (*print_key)(void *key),
               void (*print_value)(void *value));

void bst_destroy(struct bst *bst);
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "defines.h"

/**
 * @file list.h
 * @brief the definition of a list.
 * this list implementation have a larger memory footprint then the other most common ones. however this particular
 * implementation cuts down on allocation calls and maintains a bit of cache locallity
 */

struct l_vec {
  size_t _n_elem;
  size_t _capacity;
  size_t _data_size;
  void *_data;
};

struct list {
  size_t _data_size;

  intmax_t _head_idx;
  intmax_t _tail_idx;

  void (*_destroy)(void *data);

  struct l_vec _nodes;
  struct l_vec _free_idx;
};

struct list list_create(size_t data_size, void (*destroy)(void *data));

void list_destroy(struct list *list);

size_t list_size(struct list const *list);

bool list_empty(struct list const *list);

bool list_prepend(struct list *restrict list, void const *restrict data);

bool list_append(struct list *restrict list, void const *restrict data);

bool list_insert_at(struct list *restrict list, void const *restrict data, size_t pos);

void *list_peek_first(struct list const *list);

void *list_peek_last(struct list const *list);

void *list_at(struct list *list, size_t pos);

void *list_remove_first(struct list *list);

void *list_remove_last(struct list *list);

void *list_remove_at(struct list *list, size_t pos);

void *list_replace_at(struct list *restrict list, void const *restrict data, size_t pos);

intmax_t list_index_of(struct list const *restrict list,
                       void const *restrict data,
                       int (*cmpr)(void const *, void const *));

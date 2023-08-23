#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @file list
 *
 */

struct node;
struct free_table;

/**
 * @brief
 * a list implementaion where there's a slight allocation gain on push.
 */
struct list {
  size_t _num_elem;
  size_t _elem_size;
  intmax_t _head;
  intmax_t _tail;

  struct node *_nodes;

  void (*_destroy)(void *elem);
  struct free_table *_idx_free_table;
};

struct list list_create(size_t elem_size, void (*destroy)(void *));

struct list list_destroy(struct list *list);

size_t list_size(struct list const *list);

bool list_empty(struct list const *list);

bool list_prepend(struct list *restrict list, void const *restrict element);

bool list_append(struct list *restrict list, void const *restrict element);

bool list_insert_at(struct list *restrict list, void const *restrict element, size_t pos);

void *list_at(struct list *list);

void *list_remove_first(struct list *list);

void *list_remove_last(struct list *list);

void *list_remove_at(struct list *list, size_t pos);

void *list_replace(struct list *restrict list,
                   void const *restrict old,
                   void const *restrict new,
                   int (*cmpr)(void const *, void const *));

void list_sort(struct list *list);

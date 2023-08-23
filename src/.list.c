#include "list.h"
#include <limits.h>
#include <stdlib.h>

#define INIT_SIZE 32
#define GROWTH_FACTOR 1

struct node {
  intmax_t next;
  intmax_t prev;
  void *data;
};

struct free_table {
  size_t size;
  size_t capacity;
  size_t free_idx[];
};

/* pushes a new index into free_table */
static void free_table_push(struct free_table *table, size_t idx) {
  if (!table) return;
  if (table->size >= table->capacity) return;

  table->free_idx[table->size] = idx;
  table->size++;
}

/* pops the first index in free_table */
static intmax_t free_table_pop(struct free_table *table) {
  if (!table) return;
  if (!table->size) return;

  intmax_t idx = (intmax_t)table->free_idx[0];
  table->free_idx[0] = table->free_idx[table->size - 1];
  table->size--;
}

static struct free_table *free_table_create(size_t size) {
  struct free_table *free_table = malloc(sizeof *free_table + size * sizeof *free_table->free_idx);
  if (!free_table) NULL;

  free_table->capacity = size;
  for (size_t i = 0; i < size; i++) {
    free_table_push(free_table, i);
  }

  return free_table;
}

static struct free_table *free_table_resize(struct free_table *table, size_t start_idx, size_t end_idx) {
  if (!table) return NULL;
  if (start_idx > end_idx) return table;

  size_t new_capacity = table->capacity << GROWTH_FACTOR;
  if (new_capacity > INTMAX_MAX) return table;

  struct free_table *resized = realloc(table, sizeof *table + new_capacity * sizeof *table->free_idx);
  if (!resized) return table;

  resized->capacity = new_capacity;
  for (size_t i = start_idx; i < end_idx; i++) {
    free_table_push(resized, i);
  }

  return resized;
}

struct list list_create(size_t elem_size, void (*destroy)(void *)) {
  if (!elem_size || !destroy) return (struct list){0};

  struct free_table *_free_table = free_table_create(INIT_SIZE);
  if (!_free_table) return (struct list){0};

  struct nodes *_nodes = malloc(INIT_SIZE * sizeof(struct node));
  if (!_nodes) {
    free(_free_table);
    return (struct list){0};
  }

  return (struct list){._destroy = destroy,
                       ._elem_size = elem_size,
                       ._head = -1,
                       ._tail = -1,
                       ._nodes = _nodes,
                       ._num_elem = 0,
                       ._idx_free_table = _free_table};
}

struct list list_destroy(struct list *list) {
  if (!list) return;
  if (list->_head < 0) return;

  for (struct node *curr = list->_nodes + list->_head * list->_elem_size; curr->next > 0;
       curr = list->_nodes + curr->next * list->_elem_size) {
    if (list->_destroy) { list->_destroy(curr->data); }
  }

  free(list->_nodes);
  free(list->_idx_free_table);
}

size_t list_size(struct list const *list) {
  return list->_num_elem;
}

bool list_empty(struct list const *list) {
  return list->_num_elem == 0;
}

bool list_prepend(struct list *restrict list, void const *restrict element) {
  if (!list || !element) return false;
}

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
#include "list.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* node object */
struct node {
  intmax_t next;
  intmax_t prev;
  void *data;
};

static struct l_vec l_vec_create(size_t data_size) {
  if (!data_size) goto l_vec_invalid;

  void *data = malloc(VECT_INIT_CAPACITY * data_size);
  if (!data) goto l_vec_invalid;

  return (struct l_vec){._capacity = VECT_INIT_CAPACITY, ._n_elem = 0, ._data_size = data_size, ._data = data};

l_vec_invalid:
  return (struct l_vec){0};
}

static void l_vec_destroy(struct l_vec *vec) {
  if (!vec || !vec->_data) return;

  free(vec->_data);
}

static bool l_vec_empty(struct l_vec *vec) {
  if (!vec || !vec->_data) return false;

  return vec->_n_elem == 0;
}

static bool l_vec_resize(struct l_vec *vec) {
  size_t new_capacity = vec->_capacity << GROWTH_FACTOR;
  void *tmp = realloc(vec->_data, new_capacity * vec->_data_size);
  if (!tmp) return false;

  vec->_capacity = new_capacity;
  vec->_data = tmp;
  return true;
}

static void *l_vec_at(struct l_vec *vec, size_t pos) {
  if (!vec || !vec->_data) return NULL;
  if (pos >= vec->_n_elem) return NULL;

  return (char *)vec->_data + pos * vec->_data_size;
}

static bool idx_vec_resize(struct l_vec *vec) {
  if (!vec || !vec->_data) return false;

  if (!l_vec_resize(vec)) return false;

  for (size_t i = vec->_n_elem; i < vec->_capacity; i++) {
    intmax_t *curr = l_vec_at(vec, i);
    if (curr) *curr = (intmax_t)i;
  }
  return true;
}

static bool idx_vec_push(struct l_vec *vec, intmax_t idx) {
  if (!vec || !vec->_data) return false;
  if (vec->_n_elem == vec->_capacity) {
    if (!idx_vec_resize(vec)) return false;
  }

  memcpy((char *)vec->_data + vec->_n_elem * vec->_data_size, &idx, vec->_data_size);
  vec->_n_elem++;

  return true;
}

static intmax_t idx_vec_pop(struct l_vec *vec) {
  if (!vec || !vec->_data) return -1;
  if (l_vec_empty(vec)) return -1;

  intmax_t first = -1;
  memcpy(&first, vec->_data, vec->_data_size);  // prefer memcpy over ptr casts

  vec->_n_elem--;
  memmove(vec->_data, (char *)vec->_data + vec->_n_elem * vec->_data_size, vec->_data_size);
  return first;
}

static bool nodes_vec_resize(struct l_vec *vec) {
  if (!vec || !vec->_data) return false;

  if (!l_vec_resize(vec)) return false;

  // populate the new allocated space with 'empty' nodes
  for (size_t i = vec->_n_elem; i < vec->_capacity; i++) {
    struct node *curr = l_vec_at(vec, i);
    if (curr) *curr = (struct node){.data = NULL, .next = -1, .prev = -1};
  }
  vec->_n_elem = vec->_capacity;

  return true;
}

static bool nodes_vec_push(struct l_vec *restrict vec, struct node const *restrict node) {
  if (!vec || !vec->_data) return false;
  if (!node) return false;
  if (vec->_data_size != sizeof *node) return false;

  if (vec->_n_elem >= vec->_capacity) {
    if (!nodes_vec_resize(vec)) return false;
  }

  memcpy((char *)vec->_data + vec->_n_elem * vec->_data_size, node, vec->_data_size);
  vec->_n_elem++;
  return true;
}

struct list list_create(size_t data_size, void (*destroy)(void *data)) {
  if (!data_size) goto list_invalid;

  struct l_vec free_idx = l_vec_create(sizeof(intmax_t));
  if (!free_idx._capacity) goto list_invalid;

  // populate free_idx
  for (size_t i = 0; i < free_idx._capacity; i++) {
    idx_vec_push(&free_idx, (intmax_t)i);
  }

  struct l_vec nodes = l_vec_create(sizeof(struct node));
  if (!nodes._capacity) {
    l_vec_destroy(&free_idx);
    goto list_invalid;
  }

  // populate nodes
  for (size_t i = 0; i < nodes._capacity; i++) {
    nodes_vec_push(&nodes, &(struct node){.data = NULL, .next = -1, .prev = -1});
  }

  return (struct list){._data_size = data_size,
                       ._destroy = destroy,
                       ._free_idx = free_idx,
                       ._nodes = nodes,
                       ._head_idx = -1,
                       ._tail_idx = -1};

list_invalid:
  return (struct list){0};
}

void list_destroy(struct list *list) {
  if (!list) return;

  for (struct node *tmp = l_vec_at(&list->_nodes, (size_t)list->_head_idx); tmp;
       tmp = l_vec_at(&list->_nodes, (size_t)tmp->next)) {

    if (tmp->data) {
      if (list->_destroy) list->_destroy(tmp->data);
      free(tmp->data);
    }
  }

  l_vec_destroy(&list->_nodes);
  l_vec_destroy(&list->_free_idx);
}

size_t list_size(struct list const *list) {
  return list ? list->_free_idx._capacity - list->_free_idx._n_elem : 0;
}

bool list_empty(struct list const *list) {
  return list ? list->_free_idx._capacity - list->_free_idx._n_elem == 0 : true;
}

static bool list_resize(struct list *list) {
  if (!list) return false;
  // list will not exceed INTMAX_MAX
  if (list->_free_idx._capacity << GROWTH_FACTOR >= INTMAX_MAX) return false;

  if (!nodes_vec_resize(&list->_nodes)) return false;
  if (!idx_vec_resize(&list->_free_idx)) return false;
  return true;
}

static bool node_init(struct node *restrict node, void const *restrict data, size_t data_size) {
  if (!node) return false;
  if (!data || !data_size) return false;

  node->data = malloc(data_size);
  if (!node->data) return false;

  memcpy(node->data, data, data_size);
  node->next = node->prev = -1;

  return true;
}

bool list_prepend(struct list *restrict list, void const *restrict data) {
  if (!list) return false;
  if (!data) return false;

  if (l_vec_empty(&list->_free_idx)) {
    if (!list_resize(list)) return false;
  }

  intmax_t idx = idx_vec_pop(&list->_free_idx);
  if (idx < 0) return false;  // should never happen

  struct node *node = l_vec_at(&list->_nodes, (size_t)idx);
  if (!node_init(node, data, list->_data_size)) {
    idx_vec_push(&list->_free_idx, idx);
    return false;
  }

  node->next = list->_head_idx;

  struct node *head = l_vec_at(&list->_nodes, (size_t)list->_head_idx);
  if (head) {  // list has at least 1 element
    head->prev = idx;
  } else {  // list is empty
    list->_tail_idx = idx;
  }

  list->_head_idx = idx;

  return true;
}

bool list_append(struct list *restrict list, void const *restrict data) {
  if (!list) return false;
  if (!data) return false;

  if (l_vec_empty(&list->_free_idx)) {
    if (!list_resize(list)) return false;
  }

  intmax_t idx = idx_vec_pop(&list->_free_idx);
  if (idx < 0) return false;  // should never happen

  struct node *node = l_vec_at(&list->_nodes, (size_t)idx);
  if (!node_init(node, data, list->_data_size)) {
    idx_vec_push(&list->_free_idx, idx);
    return false;
  }

  node->prev = list->_tail_idx;

  struct node *tail = l_vec_at(&list->_nodes, (size_t)list->_tail_idx);
  if (tail) {  // list has at least 1 element
    tail->next = idx;
  } else {  // list is empty
    list->_head_idx = idx;
  }

  list->_tail_idx = idx;
  return true;
}

bool list_insert_at(struct list *restrict list, void const *restrict data, size_t pos) {
  if (!list) return false;
  if (!data) return false;
  if (pos > list_size(list)) return false;

  if (pos == 0) {
    return list_prepend(list, data);
  } else if (pos == list_size(list)) {
    return list_append(list, data);
  }

  if (l_vec_empty(&list->_free_idx)) {
    if (!list_resize(list)) return false;
  }

  intmax_t idx = idx_vec_pop(&list->_free_idx);
  if (idx < 0) return false;  // should never happen

  struct node *node = l_vec_at(&list->_nodes, (size_t)idx);
  if (!node_init(node, data, list->_data_size)) {
    idx_vec_push(&list->_free_idx, idx);
    return false;
  }

  size_t count = 0;
  struct node *curr = l_vec_at(&list->_nodes, list->_head_idx);
  for (; curr; curr = l_vec_at(&list->_nodes, curr->next)) {
    if (count == pos) break;
    count++;
  }

  // should never happen
  if (!curr) return list_append(list, data);

  struct node *before = l_vec_at(&list->_nodes, curr->prev);
  if (before) {  // curr should always have one element before it. otherwise curr is the head i.e. pos == 0
    node->next = before->next;
    before->next = idx;
  }

  node->prev = curr->prev;
  curr->prev = idx;
  return true;
}

void *list_peek_first(struct list const *list) {
  if (!list) return NULL;

  struct node *head = l_vec_at((struct l_vec *)&list->_nodes, list->_head_idx);
  return head ? head->data : NULL;
}

void *list_peek_last(struct list const *list) {
  if (!list) return NULL;

  struct node *tail = l_vec_at((struct l_vec *)&list->_nodes, list->_tail_idx);
  return tail ? tail->data : NULL;
}

void *list_at(struct list *list, size_t pos) {
  if (!list) return NULL;
  if (list_empty(list)) return NULL;
  if (pos >= list_size(list)) return NULL;

  if (pos == 0) {
    return list_peek_first(list);
  } else if (pos == list_size(list) - 1) {  // will never overflow
    return list_peek_last(list);
  }

  size_t count = 0;
  for (struct node *curr = l_vec_at(&list->_nodes, list->_head_idx); curr; curr = l_vec_at(&list->_nodes, curr->next)) {
    if (count == pos) return curr->data;
    count++;
  }

  return NULL;
}

void *list_remove_first(struct list *list) {
  if (!list) return NULL;
  if (list_empty(list)) return NULL;

  struct node *head = l_vec_at(&list->_nodes, list->_head_idx);
  if (!head) return NULL;

  // 'return' the index to the list of free indices
  if (!idx_vec_push(&list->_free_idx, list->_head_idx)) return NULL;

  struct node *new_head = l_vec_at(&list->_nodes, head->next);
  if (new_head) new_head->prev = -1;

  // only one element in the list
  if (list->_head_idx == list->_tail_idx) { list->_tail_idx = head->prev; }

  list->_head_idx = head->next;
  head->next = -1;

  return head->data;
}

void *list_remove_last(struct list *list) {
  if (!list) return NULL;
  if (list_empty(list)) return NULL;

  struct node *tail = l_vec_at(&list->_nodes, list->_tail_idx);
  if (!tail) return NULL;

  // 'return' the index to the list of free indices
  if (!idx_vec_push(&list->_free_idx, list->_tail_idx)) return NULL;

  struct node *new_tail = l_vec_at(&list->_nodes, tail->prev);
  if (new_tail) new_tail->next = -1;

  // only one element in the list
  if (list->_head_idx == list->_tail_idx) { list->_head_idx = tail->prev; }

  list->_tail_idx = tail->prev;
  tail->prev = -1;

  return tail->data;
}

void *list_remove_at(struct list *list, size_t pos) {
  if (!list) return NULL;
  if (list_empty(list)) return NULL;
  if (pos >= list_size(list)) return NULL;

  if (pos == 0) {
    return list_remove_first(list);
  } else if (pos == list_size(list) - 1) {
    return list_remove_last(list);
  }

  size_t count = 0;
  struct node *curr = l_vec_at(&list->_nodes, list->_head_idx);
  for (; curr; curr = l_vec_at(&list->_nodes, curr->next)) {
    if (count == pos) break;
    count++;
  }

  if (!curr) return NULL;

  struct node *before = l_vec_at(&list->_nodes, curr->prev);
  struct node *after = l_vec_at(&list->_nodes, curr->next);

  intmax_t curr_idx = -1;
  if (before) {
    curr_idx = before->next;
    before->next = curr->next;
  }

  if (after) {
    curr_idx = after->prev;
    after->prev = curr->prev;
  }

  curr->next = curr->prev = -1;

  if (curr_idx > 0) idx_vec_push(&list->_free_idx, curr_idx);
  return curr->data;
}

void *list_replace_at(struct list *restrict list, void const *restrict data, size_t pos) {
  if (!list) return NULL;
  if (!data) return NULL;
  if (list_empty(list)) return NULL;
  if (pos >= list_size(list)) return NULL;

  size_t count = 0;
  struct node *curr = l_vec_at(&list->_nodes, list->_head_idx);
  for (; curr; curr = l_vec_at(&list->_nodes, curr->next)) {
    if (count == pos) break;
    count++;
  }

  if (!curr) return NULL;

  void *old_data = curr->data;

  curr->data = malloc(list->_data_size);
  if (!curr->data) return NULL;
  memcpy(curr->data, data, list->_data_size);

  return old_data;
}

intmax_t list_index_of(struct list const *restrict list,
                       void const *restrict data,
                       int (*cmpr)(void const *, void const *)) {
  if (!list) return -1;
  if (!data) return -1;
  if (!cmpr) return -1;

  size_t pos = 0;
  for (struct node *curr = l_vec_at((struct l_vec *)&list->_nodes, list->_head_idx); curr;
       curr = l_vec_at((struct l_vec *)&list->_nodes, curr->next)) {
    if (curr->data && cmpr(data, curr->data)) return (intmax_t)pos;
    pos++;
  }

  return -1;
}

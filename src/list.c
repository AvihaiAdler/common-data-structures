#include "list.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* node object */
struct node {
  struct node *next;
  struct node *prev;
  void *data;
};

struct list list_create(size_t data_size, void (*destroy)(void *data)) {
  return (struct list){.data_size = data_size, ._destroy = destroy};
}

void list_destroy(struct list *list) {
  if (!list) return;

  for (struct node *tmp = list->_head; list->_head; tmp = list->_head) {
    list->_head = list->_head->next;

    if (list->_destroy) { list->_destroy(tmp->data); }

    if (tmp->data) { free(tmp->data); }

    free(tmp);
  }
}

/* construct a node with the a copy of the data passed in. returns a heap
 * allocated node on success, NULL on failure. internal use only */
static struct node *node_create(void const *data, size_t data_size) {
  if (data_size == 0) return NULL;

  struct node *node = calloc(1, sizeof *node);
  if (!node) return NULL;

  node->data = malloc(data_size);
  // node->data_size = data_size;
  if (!node->data) {
    free(node);
    return NULL;
  }

  memcpy(node->data, data, data_size);
  return node;
}

size_t list_size(struct list const *list) {
  return list ? list->_n_elem : 0;
}

bool list_empty(struct list const *list) {
  return list ? list->_n_elem == 0 : true;
}

bool list_prepend(struct list *restrict list, void const *restrict data, size_t data_size) {
  if (!list) return false;
  if (list->_n_elem == (SIZE_MAX >> 1)) return false;

  if (data_size != list->data_size) return false;

  struct node *tmp = node_create(data, data_size);
  if (!tmp) return false;

  if (!list->_head) {
    list->_tail = tmp;
  } else {
    list->_head->prev = tmp;
  }

  tmp->next = list->_head;
  list->_head = tmp;
  list->_n_elem++;
  return true;
}

bool list_append(struct list *restrict list, void const *restrict data, size_t data_size) {
  if (!list) return false;
  if (list->_n_elem == (SIZE_MAX >> 1)) return false;

  if (data_size != list->data_size) return false;

  struct node *tmp = node_create(data, data_size);
  if (!tmp) return false;

  if (!list->_tail) {
    list->_head = tmp;
  } else {
    list->_tail->next = tmp;
  }

  tmp->prev = list->_tail;
  list->_tail = tmp;
  list->_n_elem++;
  return true;
}

bool list_insert_at(struct list *restrict list, void const *restrict data, size_t data_size, size_t pos) {
  if (!list) return false;
  if (pos > list->_n_elem) return false;
  if (list->_n_elem == (SIZE_MAX >> 1)) return false;

  if (data_size != list->data_size) return false;

  if (pos == 0) return list_prepend(list, data, data_size);
  if (pos == list->_n_elem) return list_append(list, data, data_size);

  struct node *new_node = node_create(data, data_size);
  if (!new_node) return false;

  struct node *tmp = list->_head;
  for (size_t i = 0; i < pos - 1; i++) {
    tmp = tmp->next;
  }

  new_node->prev = tmp;
  new_node->next = tmp->next;
  tmp->next->prev = tmp;
  tmp->next = new_node;
  list->_n_elem++;
  return true;
}

bool list_insert_priority(struct list *restrict list,
                          const void *restrict data,
                          size_t data_size,
                          int (*cmpr)(void const *, void const *)) {
  if (!list) return false;
  if (list->_n_elem == (SIZE_MAX >> 1)) return false;

  if (data_size != list->data_size) return false;

  if (!list->_head) return list_prepend(list, data, data_size);  // list is empty

  if (!cmpr) return false;

  // find the node where data > node::data
  struct node *tmp = list->_head;
  for (; tmp; tmp = tmp->next) {
    if (cmpr(data, tmp->data) > 0) break;
  }

  // reached the end. all elements are bigger than the new data
  if (!tmp) return list_append(list, data, data_size);

  // the first element is smaller than the new data
  if (tmp == list->_head) return list_prepend(list, data, data_size);

  struct node *new_node = node_create(data, data_size);
  if (!new_node) return false;

  new_node->next = tmp;
  new_node->prev = tmp->prev;
  tmp->prev->next = new_node;
  tmp->prev = new_node;
  list->_n_elem++;
  return true;
}

void *list_peek_first(struct list *list) {
  if (!list || !list->_head) return NULL;

  return list->_head->data;
}

void *list_peek_last(struct list *list) {
  if (!list || !list->_tail) return NULL;

  return list->_tail->data;
}

void *list_at(struct list *list, size_t pos) {
  if (!list || !list->_head) return NULL;
  if (pos >= list->_n_elem) return NULL;

  struct node *tmp = list->_head;
  for (size_t counter = 0; counter < pos; counter++) {
    tmp = tmp->next;
  }
  return tmp->data;
}

void *list_remove_first(struct list *list) {
  if (!list || !list->_head) return NULL;

  struct node *tmp = list->_head;

  // only has 1 element
  if (list->_head == list->_tail) {
    list->_tail = list->_head = NULL;
  } else {
    list->_head = list->_head->next;
    list->_head->prev = NULL;
  }
  list->_n_elem--;

  void *data = tmp->data;
  free(tmp);
  return data;
}

void *list_remove_last(struct list *list) {
  if (!list || !list->_tail) return NULL;

  struct node *tmp = list->_tail;

  // only 1 element
  if (list->_tail == list->_head) {
    list->_head = list->_tail = NULL;
  } else {
    list->_tail = list->_tail->prev;
    list->_tail->next = NULL;
  }
  list->_n_elem--;

  void *data = tmp->data;
  free(tmp);
  return data;
}

void *list_remove_at(struct list *list, size_t pos) {
  if (!list) return NULL;
  if (pos >= list->_n_elem) return NULL;

  if (pos == 0) return list_remove_first(list);
  if (pos == list->_n_elem - 1) return list_remove_last(list);

  struct node *tmp = list->_head;
  for (size_t i = 0; i < pos; i++) {
    tmp = tmp->next;
  }

  tmp->prev->next = tmp->next;
  tmp->next->prev = tmp->prev;
  list->_n_elem--;

  void *data = tmp->data;
  free(tmp);
  return data;
}

int list_index_of(struct list *restrict list, const void *restrict data, int (*cmpr)(const void *, const void *)) {
  if (!list || !list->_head) return DS_ERROR;
  if (!cmpr) return DS_ERROR;

  size_t pos = 0;
  for (struct node *tmp = list->_head; tmp; tmp = tmp->next, pos++) {
    if (cmpr(tmp->data, data) == 0) return pos;
  }
  return DS_ERROR;
}

void *list_replace_at(struct list *restrict list, const void *restrict data, size_t data_size, size_t pos) {
  if (!list || !list->_head) return NULL;
  if (pos >= list->_n_elem) return NULL;
  if (data_size != list->data_size) return NULL;
  if (data_size == 0) return NULL;

  struct node *tmp = list->_head;
  for (size_t i = 0; i < pos; i++) {
    tmp = tmp->next;
  }

  void *old_data = malloc(list->data_size);
  if (!old_data) return NULL;

  memcpy(old_data, tmp->data, list->data_size);

  if (data_size != list->data_size) {
    void *tmp_buf = realloc(tmp->data, data_size);
    if (!tmp_buf) {
      free(old_data);
      return NULL;
    }
    tmp->data = tmp_buf;
    // tmp->data_size = data_size;
  }

  memcpy(tmp->data, data, list->data_size);
  return old_data;
}

/* replaces the first occurence of old_data with new_data. returns a pointer to
 * old_data on success, NULL otherwise */
void *list_replace(struct list *restrict list,
                   void const *restrict old_data,
                   void const *restrict new_data,
                   size_t new_data_size,
                   int (*cmpr)(const void *, const void *)) {
  size_t pos = list_index_of(list, old_data, cmpr);

  return list_replace_at(list, new_data, new_data_size, pos);
}

static struct node *list_merge(struct node *front, struct node *back, int (*cmpr)(const void *, const void *)) {
  struct node *merged = NULL;

  // base
  if (!front) return back;
  if (!back) return front;

  if (cmpr(front->data, back->data) <= 0) {
    merged = front;
    merged->next = list_merge(front->next, back, cmpr);
  } else {
    merged = back;
    merged->next = list_merge(front, back->next, cmpr);
  }

  merged->prev = merged->next->prev;
  return merged;
}

static struct node *get_middle(struct node *head) {
  struct node *slow = head;
  struct node *fast = head;

  while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
  }

  return slow;
}

static struct node *sort(struct node *head, int (*cmpr)(void const *, void const *)) {
  if (!head || !head->next) { return head; }

  struct node *middle = get_middle(head);

  // split the list
  struct node *front = head;
  struct node *back = middle;
  if (middle->prev) middle->prev->next = NULL;
  if (middle) middle->prev = NULL;

  front = sort(front, cmpr);
  back = sort(back, cmpr);

  return list_merge(front, back, cmpr);
}

/* sorts the list */
void list_sort(struct list *list, int (*cmpr)(void const *, void const *)) {
  if (!list || !list->_head) return;
  if (!cmpr) return;

  list->_head = sort(list->_head, cmpr);
}

#include "include/list.h"

#include <limits.h>
#include <stdlib.h>

struct list *list_init() {
  return calloc(1, sizeof(struct list));
}

void list_destroy(struct list *list, void (*destroy)(void *data)) {
  if (!list) return;

  for (struct node *tmp = list->head; list->head; tmp = list->head) {
    list->head = list->head->next;

    if (destroy) {
      destroy(tmp->data);
    }

    free(tmp);
  }
  free(list);
}

struct node *init_node(void *data) {
  struct node *node = calloc(1, sizeof *node);
  if (!node) return NULL;

  node->data = data;
  return node;
}

unsigned long long list_size(struct list *list) {
  if (!list) return 0;
  return list->size;
}

bool list_empty(struct list *list) {
  if (!list) return true;
  return list->size == 0;
}

bool list_prepend(struct list *list, void *data) {
  if (!list) return false;
  if (list->size == LLONG_MAX) return false;

  struct node *tmp = init_node(data);
  if (!tmp) return false;

  if (!list->head) {
    list->tail = tmp;
  }

  tmp->next = list->head;
  list->head = tmp;
  list->size++;
  return true;
}

bool list_append(struct list *list, void *data) {
  if (!list) return false;
  if (list->size == LLONG_MAX) return false;

  struct node *tmp = init_node(data);
  if (!tmp) return false;

  if (!list->tail) {
    list->head = tmp;
  }

  tmp->prev = list->tail;
  list->tail = tmp;
  list->size++;
  return true;
}

bool list_insert_at(struct list *list, void *data, unsigned long long pos) {
  if (!list) return false;
  if (pos > list->size) return false;
  if (list->size == LLONG_MAX) return false;

  if (pos == 0) return list_prepend(list, data);
  if (pos == list->size) return list_append(list, data);

  struct node *new_node = init_node(data);
  if (!new_node) return false;

  struct node *tmp = list->head;
  for (unsigned long i = 0; i < pos - 1; i++) {
    tmp = tmp->next;
  }

  new_node->prev = tmp;
  new_node->next = tmp->next;
  tmp->next->prev = tmp;
  tmp->next = new_node;
  list->size++;
  return true;
}

bool list_insert_priority(struct list *list, void *data,
                          int (*cmpr)(const void *, const void *)) {
  if (!list) return false;
  if (list->size == LLONG_MAX) return false;

  if (!list->head) return list_prepend(list, data);  // list is empty

  if (!cmpr) return false;

  // find the node where data > node::data
  struct node *tmp = list->head;
  for (; tmp; tmp = tmp->next) {
    if (cmpr(data, tmp->data) <= 0) continue;
  }

  // reached the end. all elements are bigger than the new data
  if (!tmp) return list_append(list, data);

  // the first element is smaller than the new data
  if (tmp == list->head) return list_prepend(list, data);

  struct node *new_node = init_node(data);
  if (!new_node) return false;

  new_node->next = tmp;
  new_node->prev = tmp->prev;
  tmp->prev->next = new_node;
  tmp->prev = new_node;
  list->size++;
  return true;
}

void *list_remove_first(struct list *list) {
  if (!list) return NULL;
  if (!list->head) return NULL;

  struct node *tmp = list->head;

  // only has 1 element
  if (list->head == list->tail) {
    list->tail = list->head = NULL;
  } else {
    list->head = list->head->next;
  }
  list->size--;

  void *data = tmp->data;
  free(tmp);
  return data;
}

void *list_remove_last(struct list *list) {
  if (!list) return NULL;
  if (!list->tail) return NULL;

  struct node *tmp = list->tail;

  // only 1 element
  if (list->tail == list->head) {
    list->head = list->tail = NULL;
  } else {
    list->tail = list->tail->prev;
  }
  list->size--;

  void *data = tmp->data;
  free(tmp);
  return data;
}

void *list_remove_at(struct list *list, unsigned long long pos) {
  if (!list) return NULL;
  if (pos >= list->size) return NULL;

  if (pos == 0) return list_remove_first(list);
  if (pos == list->size - 1) return list_remove_last(list);

  struct node *tmp = list->head;
  for (unsigned long i = 0; i < pos; i++) {
    tmp = tmp->next;
  }

  tmp->prev->next = tmp->next;
  tmp->next->prev = tmp->prev;
  list->size--;

  void *data = tmp->data;
  free(tmp);
  return data;
}

long long list_index_of(struct list *list, void *data,
                        bool (*equals)(const void *, const void *)) {
  if (!list) return INVALID_POS;
  if (!equals) return INVALID_POS;
  if (!list->head) return INVALID_POS;

  long long pos = 0;
  for (struct node *tmp = list->head; tmp; tmp = tmp->next, pos++) {
    if (equals(tmp->data, data)) return pos;
  }
  return INVALID_POS;
}

void *list_replace_at(struct list *list, void *data, unsigned long long pos) {
  if (!list) return NULL;
  if (!list->head) return NULL;
  if (pos < 0) return NULL;
  if (pos >= list->size) return NULL;

  struct node *tmp = list->head;
  for (unsigned long i = 0; i < pos; i++) {
    tmp = tmp->next;
  }

  void *old_data = tmp->data;
  tmp->data = data;
  return old_data;
}

/* replaces the first occurence of old_data with new_data. returns a pointer to
 * old_data on success, NULL otherwise */
void *list_replace(struct list *list, void *old_data, void *new_data,
                   bool (*equals)(const void *, const void *)) {
  long long pos = list_index_of(list, old_data, equals);
  if (pos < 0) return NULL;

  return list_replace_at(list, new_data, (unsigned long long)pos);
}

/* sorts the list */
void list_sort(struct list *list, int (*cmpr)(const void *, const void *)) {
  if (!list) return;
  if (!cmpr) return;
  if (!list->head) return;

  sort(&list->head, cmpr);
}

void sort(struct node **src, int (*cmpr)(const void *, const void *)) {
  struct node *head = *src;
  struct node *front;
  struct node *back;
  if (!head || !head->next) return;

  list_split(head, &front, &back);

  sort(&front, cmpr);
  sort(&back, cmpr);

  *src = list_merge(front, back, cmpr);
}

void list_split(struct node *src, struct node **front, struct node **back) {
  struct node *slow = src;
  struct node *fast = slow->next;

  for (; fast; fast = fast->next) {
    fast = fast->next;
    if (fast) {
      slow = slow->next;
    }
  }

  // slow reached the middle of the list;
  if (slow->next) {
    slow->next->prev = NULL;
  }

  *front = src;
  *back = slow->next;

  slow->next = NULL;
}

static struct node *list_merge(struct node *front, struct node *back,
                               int (*cmpr)(const void *, const void *)) {
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
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define GENERATE_LINKED_LIST(TYPE, NAME)                                       \
  struct NAME##_node {                                                         \
    TYPE data;                                                                 \
    struct NAME##_node *next;                                                  \
    struct NAME##_node *prev;                                                  \
  };                                                                           \
                                                                               \
  struct NAME##_linked_list {                                                  \
    size_t size;                                                               \
    struct NAME##_node *head;                                                  \
    struct NAME##_node *tail;                                                  \
  };                                                                           \
                                                                               \
  struct NAME##_node *NAME##_node_init(TYPE data) {                            \
    struct NAME##_node *node = calloc(1, sizeof *node);                        \
    if (!node) return NULL;                                                    \
    node->data = data;                                                         \
    return node;                                                               \
  }                                                                            \
                                                                               \
  struct NAME##_linked_list *NAME##_ll_init(void) {                            \
    return calloc(1, sizeof(struct NAME##_linked_list));                       \
  }                                                                            \
                                                                               \
  void NAME##_ll_destroy(struct NAME##_linked_list *ll,                        \
                         void (*NAME##_destroy)(void *data)) {                 \
    if (!ll) return;                                                           \
    for (struct NAME##_node *tmp = ll->head; tmp; tmp = ll->head) {            \
      ll->head = ll->head->next;                                               \
      if (NAME##_destroy) {                                                    \
        NAME##_destroy(tmp->data);                                             \
      }                                                                        \
      free(tmp);                                                               \
    }                                                                          \
    free(ll);                                                                  \
  }                                                                            \
  bool NAME##_ll_prepend(struct NAME##_linked_list *ll, TYPE data) {           \
    if (!ll) return false;                                                     \
                                                                               \
    struct NAME##_node *node = NAME##_node_init(data);                         \
    if (!node) return false;                                                   \
                                                                               \
    if (!ll->head) {                                                           \
      ll->head = ll->tail = node;                                              \
    } else {                                                                   \
      node->next = ll->head;                                                   \
      ll->head->prev = node;                                                   \
      ll->head = node;                                                         \
    }                                                                          \
    ll->size++;                                                                \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool append(struct linked_list *ll, TYPE data) {                             \
    if (!ll) return false;                                                     \
                                                                               \
    struct NAME##_node *node = NAME##_node_init(data);                         \
    if (!node) return false;                                                   \
                                                                               \
    if (!ll->tail) {                                                           \
      ll->tail = ll->head = node;                                              \
    } else {                                                                   \
      node->prev = ll->tail;                                                   \
      ll->tail->next = node;                                                   \
      ll->tail = node;                                                         \
    }                                                                          \
    ll->size++;                                                                \
    return true                                                                \
  }                                                                            \
                                                                               \
  bool NAME##_ll_insert_higher(struct NAME##_linked_list *ll, TYPE data,       \
                               int (*comparator)(TYPE * a, TYPE * b)) {        \
    if (!ll) return false;                                                     \
    if (!comparator) return false;                                             \
                                                                               \
    struct NAME##_node *node = NAME##_node_init(data);                         \
    if (!node) return false;                                                   \
                                                                               \
    struct NAME##_node *tmp = ll->head;                                        \
    for (; tmp && comparator(tmp->data, data) <= 0; tmp = tmp->next) {         \
      continue;                                                                \
    }                                                                          \
    if (tmp->next) tmp->next->prev = node;                                     \
    if (tmp->prev) tmp->prev->next = node;                                     \
    if (tmp == ll->head) ll->head = node;                                      \
    if (tmp == ll->tail) ll->tail = node;                                      \
    return true;                                                               \
  }                                                                            \
                                                                               \
  struct NAME##_node *NAME##_ll_remove(struct NAME##_linked_list *ll,          \
                                       TYPE data,                              \
                                       bool (*equal_to)(TYPE * a, TYPE * b)) { \
    if (!ll) return NULL;                                                      \
    if (!equal_to) return NULL;                                                \
                                                                               \
    if (ll->size == 1) {                                                       \
      struct NAME##_node *tmp = ll->head;                                      \
      ll->head = ll->tail = NULL;                                              \
      return tmp;                                                              \
    }                                                                          \
                                                                               \
    for (struct NAME##_node *tmp = ll->head; tmp; tmp = tmp->next) {           \
      if (equal_to(tmp->data, data)) {                                         \
        if (tmp->next) tmp->next->prev = tmp->prev;                            \
        if (tmp->prev) tmp->prev->next = tmp->next;                            \
        if (tmp == ll->head) ll->head = tmp->next;                             \
        if (tmp == ll->tail) ll->tail = tmp->prev;                             \
        ll->size--;                                                            \
        tmp->next = tmp->prev = NULL;                                          \
        return tmp;                                                            \
      }                                                                        \
    }                                                                          \
    return NULL;                                                               \
  }                                                                            \
                                                                               \
  struct NAME##_node *NAME##_ll_replace(                                       \
      TYPE old_data, TYPE new_data, bool (*equal_to)(TYPE * a, TYPE * b)) {    \
    if (!ll) return NULL;                                                      \
    if (!equal_to) return NULL;                                                \
                                                                               \
    struct NAME##_node *node = NAME##_node_init(new_data);                     \
    if (!node) return NULL;                                                    \
                                                                               \
    if (ll->size == 1) {                                                       \
      struct NAME##_node *tmp = ll->head;                                      \
      ll->head = ll->tail = node;                                              \
      return tmp;                                                              \
    }                                                                          \
                                                                               \
    for (struct NAME##_node *tmp = ll->head; tmp; tmp = tmp->next) {           \
      if (equal_to(tmp->data, old_data)) {                                     \
        node->next = tmp->next;                                                \
        node->prev = tmp->prev;                                                \
        if (tmp->next) tmp->next->prev = node;                                 \
        if (tmp->prev) tmp->prev->next = node;                                 \
        if (tmp == ll->head) ll->head = node;                                  \
        if (tmp == ll->tail) ll->tail = node;                                  \
        tmp->next = tmp->prev = NULL;                                          \
        return tmp;                                                            \
      }                                                                        \
    }                                                                          \
    return NULL;                                                               \
  }
#endif
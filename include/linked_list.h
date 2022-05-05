#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* generate a linked list header based on the TYPE and the NAME you pass in. a
 * TYPE must be a c type (int, char, struct some_struct, etc), NAME can be any
 * sequence of characters whihtout any whitespace characters, and MUST starts
 * with a letter */
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
  /* initialize a node with TYPE data member. return the node on success or    \
   * NULL on failure. node->next and node->tail initialize as NULL */          \
  struct NAME##_node *NAME##_node_init(TYPE data) {                            \
    struct NAME##_node *node = calloc(1, sizeof *node);                        \
    if (!node) return NULL;                                                    \
    memmove(&node->data, &data, sizeof(TYPE));                                 \
    return node;                                                               \
  }                                                                            \
                                                                               \
  /* initialize a linked list struct. return the struct on success or NULL on  \
   * failure. list->head and list->tail are initialize as NULL. list->size as  \
   * 0 */                                                                      \
  struct NAME##_linked_list *NAME##_ll_init(void) {                            \
    return calloc(1, sizeof(struct NAME##_linked_list));                       \
  }                                                                            \
                                                                               \
  /* destroy a linked list 'object'. expects a destroy function which suppose  \
   * to destroy node::data. pass in NULL if node::data is stack allocated */   \
  void NAME##_ll_destroy(struct NAME##_linked_list *ll,                        \
                         void (*NAME##_destroy)(TYPE data)) {                  \
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
                                                                               \
  /* add a node to the start of the list. return true on success, false        \
   * otherwise */                                                              \
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
  /* add a node to the end of the list. return true on success, false          \
   * otherwise */                                                              \
  bool NAME##_ll_append(struct NAME##_linked_list *ll, TYPE data) {            \
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
    return true;                                                               \
  }                                                                            \
                                                                               \
  /* create a new node which hold data and insert it based on a comparator     \
   * function. the new node will be inserted after all the elements below      \
   * (less than) him. return true on success, false otherwise */               \
  bool NAME##_ll_insert_higher(struct NAME##_linked_list *ll, TYPE data,       \
                               int (*comparator)(TYPE a, TYPE b)) {            \
    if (!ll) return false;                                                     \
    if (!comparator) return false;                                             \
                                                                               \
    if (!ll->head) {                                                           \
      if (NAME##_ll_prepend(ll, data)) return true;                            \
      return false;                                                            \
    }                                                                          \
                                                                               \
    struct NAME##_node *node = NAME##_node_init(data);                         \
    if (!node) return false;                                                   \
                                                                               \
    struct NAME##_node *tmp = ll->head;                                        \
    for (; tmp->next; tmp = tmp->next) {                                       \
      if (comparator(data, tmp->data) > 0) break;                              \
    }                                                                          \
                                                                               \
    if (comparator(data, tmp->data) > 0) {                                     \
      if (tmp->prev) tmp->prev->next = node;                                   \
      node->next = tmp;                                                        \
      node->prev = tmp->prev;                                                  \
      tmp->prev = node;                                                        \
      if (tmp == ll->head) ll->head = node;                                    \
    } else {                                                                   \
      node->prev = ll->tail;                                                   \
      ll->tail->next = node;                                                   \
      ll->tail = node;                                                         \
    }                                                                          \
                                                                               \
    ll->size++;                                                                \
    return true;                                                               \
  }                                                                            \
                                                                               \
  /* remove the first node from the list. returns the first node on success    \
   * (which has to be free'd), or NULL on failure */                           \
  struct NAME##_node *NAME##_ll_remove_first(struct NAME##_linked_list *ll) {  \
    if (!ll) return NULL;                                                      \
    if (!ll->head) return NULL;                                                \
                                                                               \
    struct NAME##_node *removed = ll->head;                                    \
                                                                               \
    ll->head = ll->head->next;                                                 \
    ll->head->prev = NULL;                                                     \
    removed->next = NULL;                                                      \
    ll->size--;                                                                \
    return removed;                                                            \
  }                                                                            \
                                                                               \
  /* remove the last node from the list. returns the last node on success      \
   * (which has to be free'd), or NULL on failure */                           \
  struct NAME##_node *NAME##_ll_remove_last(struct NAME##_linked_list *ll) {   \
    if (!ll) return NULL;                                                      \
    if (!ll->tail) return NULL;                                                \
                                                                               \
    struct NAME##_node *removed = ll->tail;                                    \
                                                                               \
    ll->tail = ll->tail->prev;                                                 \
    ll->tail->next = NULL;                                                     \
    removed->prev = NULL;                                                      \
    ll->size--;                                                                \
    return removed;                                                            \
  }                                                                            \
                                                                               \
  /* remove the first node which contains data from the list. return the       \
   * removed node (which has to be freed), or NULL if no such node found */    \
  struct NAME##_node *NAME##_ll_remove(struct NAME##_linked_list *ll,          \
                                       TYPE data,                              \
                                       bool (*equal_to)(TYPE a, TYPE b)) {     \
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
  /* replace the first node which contains old_data with a different node      \
   * which contains new_data. the function expects an equal_to function which  \
   * then use to deteremine which node contains old_data. retunrs the old node \
   * on success (which has to be freed), or NULL on failure */                 \
  struct NAME##_node *NAME##_ll_replace(struct NAME##_linked_list *ll,         \
                                        TYPE old_data, TYPE new_data,          \
                                        bool (*equal_to)(TYPE a, TYPE b)) {    \
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
#pragma once

#include <stdbool.h>

#define INVALID_POS -1

/* node object */
struct node {
  void *data;
  struct node *next;
  struct node *prev;
};

/* doubly linked list object */
struct list {
  unsigned long long size;  // can never exceeds LLONG_MAX
  struct node *head;
  struct node *tail;
};

/* initialize a heap allocated linked list with a size of 0, where head and tail
 * points to NULL. returns a list on success, NULL on failure
 */
struct list *list_init();

/* destroy a list 'object'. if destroy isn't NULL, calls it for every node::data
 */
void list_destroy(struct list *list, void (*destroy)(void *data));

/* construct a node with the data passed in. returns a heap allocated node on
 * success, NULL on failure. internal use only */
static struct node *init_node(void *data);

/* returns the number of elements on the list. avoid acceessing list::size
 * directly. use this method instead */
unsigned long long list_size(struct list *list);

/* returns whether vector is emtpy or not. if list is NULL - returns true */
bool list_empty(struct list *list);

/* add a node to the start of the list. returns true on success, false otherwise
 */
bool list_prepend(struct list *list, void *data);

/* add a node to the end of the list. returns true on success, false otherwise
 */
bool list_append(struct list *list, void *data);

/* constructs and inserts a node at position pos. the position is calculated
 * from the head of the list. returns true on success, false otherwise */
bool list_insert_at(struct list *list, void *data, unsigned long long pos);

/* constructs and inserts a node at a position determined by the comparator.
 * inserts the new node at the first location where new_node::data >
 * another_node::data */
bool list_insert_priority(struct list *list, void *data,
                          int (*cmpr)(const void *, const void *));

/* removes the first node from the list. returns a pointer to the removed
 * element on success, NULL on failure */
void *list_remove_first(struct list *list);

/* removes the first node from the list. returns a pointer to the removed
 * element on success, NULL on failure */
void *list_remove_last(struct list *list);

/* removes the node at position pos. the position is caluclated from the head of
 * the list. returns a pointer to the removed element on success, NULL otherwise
 */
void *list_remove_at(struct list *list, unsigned long long pos);

/* finds the first occurence of data and returns its index. the index is
 * calculated for the list's head. returns a positive number as the index on
 * success, negative number on failure */
long long list_index_of(struct list *list, void *data,
                        bool (*equals)(const void *, const void *));

/* replaces an element at position pos. the position is calculated from the
 * list's head. returns a pointer to the replcaed data on success, NULL
 * otherwise */
void *list_replace_at(struct list *list, void *data, unsigned long long pos);

/* replaces the first occurence of old_data with new_data. returns a pointer to
 * old_data on success, NULL otherwise */
void *list_replace(struct list *list, void *old_data, void *new_data,
                   bool (*equals)(const void *, const void *));

/* sorts the list */
void list_sort(struct list *list, int (*cmpr)(const void *, const void *));

static void list_split(struct node *src, struct node **front,
                       struct node **back);

static struct node *list_merge(struct node *front, struct node *back,
                               int (*cmpr)(const void *, const void *));

static void sort(struct node **src, int (*cmpr)(const void *, const void *));
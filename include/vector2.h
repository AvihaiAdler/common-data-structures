#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define GROWTH_FACTOR 1
#define INIT_CAPACITY 16

/* list object */
struct list {
  size_t size;
  size_t capacity;
  void **data;
};

/* initialize a list object. returns struct list * on success, NULL on failure.
 * elements are stored as (void *), this means you need to be aware of their
 * scope. constants can't be stored as well */
struct list *list_init();

/* destroy a list and all if it's undelying data. if (*destroy) isn't NULL call
 * it for every element in the underlying array */
void list_destroy(struct list *list, void (*destroy)(void *element));

/* returns whether list is emtpy or not */
bool list_empty(struct list *list);

/* returns the element at position pos. any changes to the element will change
 * the stored element on the list. returns NULL on failure */
void *list_at(struct list *list, size_t pos);

/* sorts the list and finds an element on the list and returns it. returns NULL
 * on failure */
void *list_find(struct list *list, void *element,
                int (*cmpr)(const void *, const void *));

/* used internally to resize the list by GROWTH_FACTOR */
static bool list_resize(struct list *list);

/* reservse space for size elements. returns the new reserved space
 * (list::capacity) */
size_t list_reserve(struct list *list, size_t size);

/* push a new element into the list. returns truw on success, false otherwise */
bool list_push(struct list *list, void *element);

/* pops an element for the end of the list. returns the poped element on
 * success. NULL on failure. if the element was heap allocated it has to be
 * free'd */
void *list_pop(struct list *list);

/* remove the element at position pos. returns the removed element. NULL on
 * failure */
void *list_remove_at(struct list *list, size_t pos);

/* replaces an element on the list at position pos. returns the
 * replaced element on success as heap allocated element (has to be free'd), or
 * NULL on failure */
void *list_replace(struct list *list, void *element, size_t pos);

/* shrink the underlying array to fit exactly list::size elements. returns the
 * new size */
size_t list_shrink(struct list *list);

/* finds and returns the index of the first occurence of an element on the list.
 * returns its position on success, or -1 if no such element found */
int64_t list_index_of(struct list *list, void *element,
                      bool (*equals)(const void *, const void *));

/* sort the list. the compr function should returns an int bigger than 0 if the
 * first element if bigger the second, 0 if both elements are equals or an int
 * smaller than 0 if the first element is smaller than the second */
void list_sort(struct list *list, int (*cmpr)(const void *, const void *));

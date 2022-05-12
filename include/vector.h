#pragma once

#include <stdbool.h>

#define GROWTH_FACTOR 1
#define INIT_CAPACITY 16

/* vector object */
struct vector {
  // both size and capacity can never exceed LLONG_MAX
  unsigned long long size;
  unsigned long long capacity;
  void **data;
};

/* initialize a vector object. returns struct vector * on success, NULL on
 * failure. elements are stored as (void *), this means you need to be aware of
 * their scope. constants can't be stored as well */
struct vector *vector_init();

/* destroy a vector and all if it's undelying data. if (*destroy) isn't NULL
 * call it for every element in the underlying array */
void vector_destroy(struct vector *vector, void (*destroy)(void *element));

/* returns whether vector is emtpy or not */
bool vector_empty(struct vector *vector);

/* returns the element at position pos. any changes to the element will change
 * the stored element on the vector. returns NULL on failure */
void *vector_at(struct vector *vector, unsigned long long pos);

/* sorts the vector and finds an element on the vector and returns it. returns
 * NULL on failure */
void *vector_find(struct vector *vector, void *element,
                  int (*cmpr)(const void *, const void *));

/* used internally to resize the vector by GROWTH_FACTOR */
static bool vector_resize(struct vector *vector);

/* reservse space for size elements. returns the new reserved space
 * (vector::capacity) */
unsigned long long vector_reserve(struct vector *vector,
                                  unsigned long long size);

/* push a new element into the vector. returns truw on success, false otherwise
 */
bool vector_push(struct vector *vector, void *element);

/* pops an element for the end of the vector. returns the poped element on
 * success. NULL on failure. if the element was heap allocated it has to be
 * free'd */
void *vector_pop(struct vector *vector);

/* remove the element at position pos. returns the removed element. NULL on
 * failure */
void *vector_remove_at(struct vector *vector, unsigned long long pos);

/* replaces an element on the vector at position pos. returns the
 * replaced element on success as heap allocated element (has to be free'd), or
 * NULL on failure */
void *vector_replace(struct vector *vector, void *element,
                     unsigned long long pos);

/* shrink the underlying array to fit exactly vector::size elements. returns the
 * new size */
unsigned long long vector_shrink(struct vector *vector);

/* finds and returns the index of the first occurence of an element on the
 * vector. returns its position on success, or -1 if no such element found */
long long vector_index_of(struct vector *vector, void *element,
                          bool (*equals)(const void *, const void *));

/* sort the vector. the compr function should returns an int bigger than 0 if
 * the first element if bigger the second, 0 if both elements are equals or an
 * int smaller than 0 if the first element is smaller than the second */
void vector_sort(struct vector *vector,
                 int (*cmpr)(const void *, const void *));

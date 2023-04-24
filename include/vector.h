#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "defines.h"

struct vec;

struct vec_iter;

/* initialize a vector object. returns struct vector * on success, NULL on
 * failure */
struct vec *vec_init(size_t data_size);

/* destroy a vector and all if it's undelying data. if (*destroy) isn't NULL
 * call it for every element in the underlying array. you should only pass in a
 * destroy function if your elements contains a pointer to a heap allocated
 * memory */
void vec_destroy(struct vec *vec, void (*destroy)(void *element));

/* returns the number of elements in the vector. avoid acceessing vector::size
 * directly. use this method instead */
size_t vec_size(struct vec *vec);

/* returns the number of elements you can fit in the vector. avoid acceessing
 * vector::capacity directly. use this method instead */
size_t vec_capacity(struct vec *vec);

/* return the underlying array of the vector. this function should be use with
 * care, changing said data may result in UB */
void *vec_data(struct vec *vec);

/* returns the size, in bytes of the vector */
size_t vec_struct_size(struct vec *vec);

/* returns whether vector is emtpy or not. if vector is NULL - returns true */
bool vec_empty(struct vec *vec);

/* returns the element at position pos. any changes to the element will change
 * the stored element on the vector. returns NULL on failure */
void *vec_at(struct vec *vec, size_t pos);

/* sorts the vector and finds an element on the vector and returns it. returns
 * NULL on failure */
void *vec_find(struct vec *vec, const void *element,
               int (*cmpr)(const void *, const void *));

/* reservse space for size elements. returns the new reserved space
 * (vector::capacity) */
size_t vec_reserve(struct vec *vec, size_t size);

/* changes the size of the vector. if size < vector::size vector::size will
 * decrease to the size passed in. beware if the vector contains a pointers to
 * heap allocated memory you might loose track of them causing a memory leak. if
 * size > vector::capacity the result will be as if vector_reserve were called
 * followed by vector_resize. if size >= vector::size && size <
 * vector::capacity, vector::size will be set to size and number of NULL values
 * will be pushed into the vector. returns the new vector::size */
size_t vec_resize(struct vec *vec, size_t size);

/* push a new element into the vector. returns true on success, false otherwise
 */
bool vec_push(struct vec *vec, const void *element);

/* pops an element for the end of the vector. returns the poped element on
 * success. NULL on failure. the element must not be free'd! however if the
 * element contains a pointer to a heap allocated memory - it (that pointer)
 * must be free'd */
void *vec_pop(struct vec *vec);

/* remove the element at position pos. returns the removed element (which has to
 * be free'd). NULL on failure */
void *vec_remove_at(struct vec *vec, size_t pos);

/* replaces an element on the vector at position pos. returns the
 * replaced element on success as heap allocated element (has to be free'd), or
 * NULL on failure */
void *vec_replace(struct vec *vec, const void *element, size_t pos);

/* shrink the underlying array to fit exactly vector::size elements. returns the
 * new capacity */
size_t vec_shrink(struct vec *vec);

/* finds and returns the index of the first occurence of an element on the
 * vector. returns its position on success, or N_EXISTS if no such element
 * found */
size_t vec_index_of(struct vec *vec, const void *element,
                    int (*cmpr)(const void *, const void *));

/* sort the vector. the compr function should returns an int bigger than 0 if
 * the first element if bigger the second, 0 if both elements are equals or an
 * int smaller than 0 if the first element is smaller than the second. */
void vec_sort(struct vec *vec, int (*cmpr)(const void *, const void *));

/* iterator related function. a vec can in any given moment have exactly 1
 * iterator. calling either vec_iter_begin or vec_iter_end to get a new iterator
 * while there's already an iterator present will override the previous iterator
 */
/* returns an iterator to the beginning of the vector */
struct vec_iter *vec_iter_begin(struct vec *vec);

/* returns an iterator to the end of the vector */
struct vec_iter *vec_iter_end(struct vec *vec);

/* returns true if the iterator hasn't reach the end */
bool vec_iter_has_next(struct vec_iter *iter);

/* advences the iterator to the next element of the vector */
struct vec_iter *vec_iter_next(struct vec_iter *iter);

/* returns true if the iterator hasn't reach the beginning */
bool vec_iter_has_prev(struct vec_iter *iter);

/* advences the iterator to the previous element of the vector */
struct vec_iter *vec_iter_prev(struct vec_iter *iter);

/* returns a pointer to the element pointed by the iterator */
void *vec_iter_get(struct vec_iter *iter);

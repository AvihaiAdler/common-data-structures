#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "defines.h"

/**
 * @file vec.h
 * @brief the definition of vector
 */
struct vec {
  // both size and capacity can never exceed SIZE_MAX / 2
  size_t _n_elem;
  size_t _capacity;
  size_t _data_size;
  void *_data;

  void (*_destroy)(void *_element);
};

/**
 * @brief constructs a `vec` object. each element in the `vec` must be of size `data_size`.
 *
 * @param[in] data_size the size of each element in the `vec`.
 * @param[in] _destroy a pointer to a function which takes a `void *` and returns `void`. if the pointer isn't `NULL` -
 * the function will be called on each of the `vec`'s elements. make sure to pass a `destroy` function if your elements
 * contains a pointer to some heap allocated data.
 *
 * @return `struct vec` - a vec object. if `data_size` is 0 a NULL will be returned. if `data_size` is too big to fit
 * `VEC_INIT_CAPACITY` elements withing the vector - a `NULL` will be returned
 */
struct vec vec_create(size_t data_size, void (*destroy)(void *_element));

/**
 * @brief destroys a `vec` object and all of its undelaying data. if `destroy` isn't `NULL` - calls it on each of the
 * elements in the `vec`.
 *
 * @param[in] vec a `vec` object to be destroyed.
 */
void vec_destroy(struct vec *vec);

/**
 * @brief returns the number of elements in the `vec`.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `size_t` - the number of elements in the `vec`.
 */
size_t vec_size(struct vec const *vec);

/**
 * @brief returns the number of elements one can fit in the `vec` vec.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `size_t` - the number of elements the `vec` can fit in it's *current* buffer.
 */
size_t vec_capacity(struct vec const *vec);

/**
 * @brief returns the undelying buffer of the `vec`. this function should use with care, as certain changes to said
 * buffer as well as operations on it (read / write / allocate / free etc) - may result in undefined behavior.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `void *` - a pointer to the underlying buffer ofthe `vec`.
 */
void *vec_data(struct vec *vec);

/**
 * @brief returns whether or not a `vec` object is empty.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `true` - if the `vec` is empty or is a `NULL` pointer.
 * @return `false` - if the `vec` holds at least one element.
 */
bool vec_empty(struct vec const *vec);

/**
 * @brief returns a pointer to the element at position `pos`. `pos` isn't a valid index - `NULL` will be returned. this
 * function should be used with care as any changes to the element will change the data stored on the `vec`. moreover,
 * certain operation on the returned pointer might corrupt other elements' data.
 *
 * @param[in] vec a `vec` object.
 * @param[in] pos the position of the desired element.
 *
 * @return `void *` - a pointer to the desired element. if `pos` exceeds the number of elements in the `vec` - a `NULL`
 * will be returned.
 */
void *vec_at(struct vec *vec, size_t pos);

/**
 * @brief finds the *first occurence* of an element in the `vec` and returns a pointer to it. this function should be
 * used with care as any changes to the element will change the data stored on the `vec`. moreover, certain operation on
 * the returned pointer might corrupt other elements' data.
 *
 * @param[in] vec a `vec` object to be searched in.
 * @param[in] element a *pointer* to an element to be looked for.
 * @param[in] cmpr a pointer to a function comparing `2` elements in the vector. this function ptr must not be `NULL`.
 * the function which takes `2` `void const *` and returns a *positive* integer if `_a > _b`,
 * *negative* integer if `_a < _b` or `0` if `_a == _b`.
 *
 * @return `void *` - a pointer to the first ocurrence of such element. if no such element was found - a `NULL` pointer
 * will be returned.
 */
void *vec_find(struct vec *restrict vec, void const *restrict element, int (*cmpr)(void const *_a, void const *_b));

/**
 * @brief reserves a space for `count` elements. returns the new `vec::capacity`.
 *
 * @param[in] vec a `vec` object.
 * @param[in] count the number of elements the `vec` should hold.
 *
 * @return `size_t` the new `vec::capacity`.
 */
size_t vec_reserve(struct vec *vec, size_t count);

/**
 * @brief resize the ector to fit at least `num_elements` elements
 *
 * @param[in] vec a `vec` object to resize.
 * @param[in] num_elements the desired number of elements the `vec` should hold.
 *
 * @return `size_t` the new `vec::capacity`.
 */
size_t vec_resize(struct vec *vec, size_t num_elements);

/**
 * @brief pushes a *copy* of `element` to the end of the `vec`.
 *
 * changes to `element` after the push won't affect the copy the `vec` holds. thus, if the element is heap allocated /
 * contains some heap allocated data - it must be free'd independently.
 *
 * @param[in] vec a `vec` object.
 * @param[in] element a pointer to the element to be pushed in.
 *
 * @return `true` on success.
 * @return `false` on failure.
 */
bool vec_push(struct vec *restrict vec, void const *restrict element);

/**
 * @brief pops the last element of the `vec`.
 *
 * the element *itself* must not be free'd, however if said element contains a pointer to some heap allocated data -
 * that pointer must be free'd indepenedtly. this function should be used with care as any changes to the element will
 * change the data stored on the `vec`. moreover, certain operation on the returned pointer might corrupt other
 * elements' data. in addition, the element might be over-written over as soon as a new element is pushed in.
 *
 * @param[in] vec a `vec` object to be popped.
 *
 * @return `void *` a pointer to the popped element.
 */
void const *vec_pop(struct vec *vec);

/**
 * @brief removes an element from `vec` at position `pos`. returns a *copy* of said element. said copy must be free'd
 * independently.
 *
 * @param[in] vec a `vec` object.
 * @param[in] pos the position the desired element is.
 *
 * @return `void *` - a pointer to a *copy* of the element or `NULL` if `pos` exceeds `vec::size`.
 */
void *vec_remove_at(struct vec *vec, size_t pos);

/**
 * @brief replaces an element in the `vec` a postion `pos`. returns a *copy* of the replaced element. said copy must be
 * free'd independently.
 *
 * @param[in] vec a `vec` object.
 * @param[in] element a pointer to the new element. the `vec` will create a *copy* of it in position `pos`. if `element`
 * is heap allocated - it must be free'd.
 * @param[in] pos the position to put the new element in.
 *
 * @return `void *` - a pointer to a *copy* of the replaced element. said copy must be free'd independently. if `pos`
 * exceeds `vec::size` - `NULL` pointer will be returned.
 */
void *vec_replace(struct vec *restrict vec, void const *restrict element, size_t pos);

/**
 * @brief shrinks the underlying buffer to fit exaclty (depends on the allocator in use) `vec::_n_elem` elements.
 * returns the new `vec::capacity`.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `size_t` the new `vec::capacity`.
 */
size_t vec_shrink(struct vec *vec);

/**
 * @brief sorts the `vec`.
 *
 * @param[in] vec a `vec` object.
 * @param[in] cmpr a pointer to a function comparing `2` elements in the vector. this function ptr must not be `NULL`.
 * the function which takes `2` `void const *` and returns a *positive* integer if `_a > _b`,
 * *negative* integer if `_a < _b` or `0` if `_a == _b`.
 */
void vec_sort(struct vec *vec, int (*cmpr)(void const *_a, void const *_b));

/* iterator related function. a vec can in any given moment have exactly 1
 * iterator. calling either vec_iter_begin or vec_iter_end to get a new iterator
 * while there's already an iterator present will override the previous iterator */

/**
 * @brief returns an iterator to the beginning of the `vec`.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `void *` the iterator. if `vec` is `NULL` a `NULL` pointer will be returned.
 */
void *vec_iter_begin(struct vec *vec);

/**
 * @brief returns an iterator to the end of the `vec`.
 *
 * @param[in] vec a `vec` object.
 *
 * @return `void *` the iterator. if `vec` is `NULL` a `NULL` pointer will be returned.
 */
void *vec_iter_end(struct vec *vec);

/**
 * @brief advances the iterator to the next element.
 *
 * @param[in] vec the `vec` to iterate over
 * @param[in] iter an iterator object.
 *
 * @return `void *` the new tramsformed iterator object. if `iter` is `NULL` or `vec` are `NULL` pointer will be
 * returned.
 */
void *vec_iter_next(struct vec *restrict vec, void *restrict iter);

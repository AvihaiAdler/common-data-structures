#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "defines.h"

/**
 * @file list.h
 * @brief the definition of a list.
 * this list implementation have a larger memory footprint then the other most common ones. however this particular
 * implementation cuts down on allocation calls and maintains a bit of cache locallity
 */

struct l_vec {
  size_t _n_elem;
  size_t _capacity;
  size_t _data_size;
  void *_data;
};

struct list {
  size_t _data_size;

  intmax_t _head_idx;
  intmax_t _tail_idx;

  void (*_destroy)(void *data);

  struct l_vec _nodes;
  struct l_vec _free_idx;
};

/**
 * @brief creates a list
 *
 * @param[in] data_size the size of each element on the list
 * @param[in] destroy a destroy function which may be `NULL`. if `destroy` isn't `NULL` it will be called on each
 * element upon list destruction
 * @return `struct list`
 */
struct list list_create(size_t data_size, void (*destroy)(void *data));

/**
 * @brief destroys a list
 *
 * @param[in] list
 */
void list_destroy(struct list *list);

/**
 * @brief returns the number of elements in a list
 *
 * @param[in] list
 * @return `size_t` the number of elements the list holds
 */
size_t list_size(struct list const *list);

/**
 * @brief returns whether a list hold no elements
 *
 * @param[in] list
 * @return `true` if the list contains no elements
 * @return `false` if the list contains at least one element
 */
bool list_empty(struct list const *list);

/**
 * @brief prepends a copy of an element to the beginning of a list
 *
 * @param[in] list
 * @param[in] data the data to prepend
 * @return `true` if the operation succeeded
 * @return `false` if the operation failed
 */
bool list_prepend(struct list *restrict list, void const *restrict data);

/**
 * @brief appends a copy of an element to the end of a list
 *
 * @param[in] list
 * @param[in] data the data to append
 * @return `true` if the operation succeeded
 * @return `false` if the operation failed
 */
bool list_append(struct list *restrict list, void const *restrict data);

/**
 * @brief inserts a copy of an element at some position in a list
 * said position must be valid. i.e. the list isn't empty and `0 <= pos < list::size`
 *
 * @param[in] list
 * @param[in] data the element to insert
 * @param[in] pos the position to insert the element into. must hold `0 <= pos < list::size`
 * @return `true` if the operation succeeded
 * @return `false` if the operation failed
 */
bool list_insert_at(struct list *restrict list, void const *restrict data, size_t pos);

/**
 * @brief returns a pointer to the first element on a list
 *
 * @param[in] list
 * @return `void *` a pointer to the first element on the list. `NULL` if the list is empty
 */
void *list_peek_first(struct list const *list);

/**
 * @brief returns a pointer to the last element on a list
 *
 * @param[in] list
 * @return `void *` a pointer to the last element on the list. `NULL` if the list is empty
 */
void *list_peek_last(struct list const *list);

/**
 * @brief returns a pointer to the element at some position on a list
 * said position must be valid. i.e. the list isn't empty and `0 <= pos < list::size`
 *
 * @param[in] list
 * @param[in] pos the position of the desired element. must hold `0 <= pos < list::size`
 * @return `void *` a pointer to the element at position `pos`. `NULL` if `pos` isn't valid or if the list is empty
 */
void *list_at(struct list *list, size_t pos);

/**
 * @brief removes the first element on a list
 *
 * @param[in] list
 * @return `void *` a pointer to the old (removed) element. `NULL` if the list is empty. said pointer must be `free`d by
 * the caller
 */
void *list_remove_first(struct list *list);

/**
 * @brief removes the last element on a list
 *
 * @param[in] list
 * @return `void *` a pointer to the old (removed) element. `NULL` if the list is empty. said pointer must be `free`d by
 * the caller
 */
void *list_remove_last(struct list *list);

/**
 * @brief removes an element at some position on a list
 * said position must be valid. i.e. the list isn't empty and `0 <= pos < list::size`
 *
 * @param[in] list
 * @param[in] pos the position of the desired element. must hold `0 <= pos < list::size`
 * @return `void *` a pointer to the old (removed) element. `NULL` if the list is empty. said pointer must be `free`d by
 * the caller
 */
void *list_remove_at(struct list *list, size_t pos);

/**
 * @brief replaces an element at some position on a list
 * said position must be valid. i.e. the list isn't empty and `0 <= pos < list::size`
 *
 * @param[in] list
 * @param[in] pos the position of the desired element. must hold `0 <= pos < list::size`
 * @return `void *` a pointer to the old (replaced) element. `NULL` if the list is empty. said pointer must be `free`d
 * by the caller
 */
void *list_replace_at(struct list *restrict list, void const *restrict data, size_t pos);

/**
 * @brief returns the index of an element on a list
 *
 * @param[in] list
 * @param[in] data the element to look for
 * @param[in] cmpr a compare function. the function must return `0` if both elements are euqal. a negative number if
 * `data < other` and a positive number if `data > other`
 * @return `intmax_t` the index of the desired element. `-1` if said element isn't on the list
 */
intmax_t list_index_of(struct list const *restrict list,
                       void const *restrict data,
                       int (*cmpr)(void const *, void const *));

/**
 * @brief returns an iterator to the first element on the list.
 * the iterator is a double pointer to said element casted into a `void **` and must be casted approprietly
 *
 * @param[in] list
 * @return `void *` a double pointer to the the first element on the list. `NULL` if the list is empty
 */
void *list_iter_begin(struct list *list);

/**
 * @brief returns an iterator to the last element on the list.
 * the iterator is a double pointer to said element casted into a `void **` and must be casted approprietly
 *
 * @param[in] list
 * @return `void *` a double pointer to the the last element on the list. `NULL` if the list is empty
 */
void *list_iter_end(struct list *list);

/**
 * @brief advances an iterator to the next element on the list. returns the new iterator
 * the iterator is a double pointer to said element casted into a `void **` and must be casted approprietly
 *
 * @param[in] list
 * @param[in] iter a valid iterator
 * @return `void *` a double pointer to the the last element on the list. `NULL` if the list is empty
 */
void *list_iter_next(struct list *restrict list, void *restrict iter);

#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "defines.h"

struct queue {
  bool _empty;
  size_t _capacity;

  size_t _start;
  size_t _end;

  void (*_destroy)(void *_element);

  size_t _data_size;
  void *_data;
};

/**
 * @brief creates am empty (unbounded) queue with some initial capacity
 *
 * @param data_size the size of each element placed in the queue
 * @param destroy a destructor for each element in the queue. may be `NULL` in which case the destructor won't be called
 * upon the destruction of the queue.
 * @return `struct queue`
 */
struct queue queue_create(size_t data_size, void (*destroy)(void *_element));

/**
 * @brief destroys a queue
 *
 * @param queue
 */
void queue_destroy(struct queue *queue);

/**
 * @brief returns the number of elements one can place in a queue. the number returned as if the queue is empty (i.e.
 * the number of elements already in the queue don't take into account)
 *
 * @param queue
 * @return `size_t`
 */
size_t queue_capacity(struct queue const *queue);

/**
 * @brief returns the number of elements currently in the queue
 *
 * @param queue
 * @return `size_t`
 */
size_t queue_size(struct queue const *queue);

/**
 * @brief returns whether or not there's at least one element in the queue
 *
 * @param queue
 * @return `true` if there's at least one element in the queue
 * @return `false` if there aren't any element in the queue
 */
bool queue_empty(struct queue const *queue);

/**
 * @brief copies the element `element` into the `queue`. said copy is shallow
 *
 * @param queue
 * @param element the element to copy into the queue
 * @return `enum ds_error` `DS_OK` on success, `DS_NO_MEM` on allocation failure or `DS_ERROR` on any other failure.
 */
enum ds_error queue_enqueue(struct queue *restrict queue, void const *restrict element);

/**
 * @brief removes the first element from the queue and places it in `element`. the destructor _isn't_ called
 *
 * @param queue
 * @param element a ptr to a valid `element` to which a shallow copy of the first element in the queue will be placed
 * @return `enum ds_error` `DS_VALUE_OK` on success, `DS_NOT_FOUND` if the queue is empty, or `DS_ERROR` on any other
 * failure.
 */
enum ds_error queue_dequeue(struct queue *restrict queue, void *restrict element);

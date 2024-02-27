#include "queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_INIT_CAPACITY 64

struct queue queue_create(size_t data_size, void (*destroy)(void *_element)) {
  if (!data_size) { goto empty_queue_lbl; }

  void *data = malloc(data_size * QUEUE_INIT_CAPACITY);
  if (!data_size) { goto empty_queue_lbl; }

  return (struct queue){._empty = true,
                        ._capacity = QUEUE_INIT_CAPACITY,
                        ._destroy = destroy,
                        ._data_size = data_size,
                        ._data = data};

empty_queue_lbl:
  return (struct queue){._empty = true};
}

void queue_destroy(struct queue *queue) {
  if (!queue) { return; }

  if (!queue->_data || !queue->_capacity) { return; }

  if (queue->_destroy) {
    for (size_t i = queue->_start; i != queue->_end; i = (i + 1) % queue->_capacity) {
      queue->_destroy((char *)queue->_data + i * queue->_data_size);
    }
  }

  free(queue->_data);
}

size_t queue_capacity(struct queue const *queue) {
  if (!queue || !queue->_data) { return 0; }

  return queue->_capacity;
}

size_t queue_size(struct queue const *queue) {
  if (!queue || !queue->_capacity) { return 0; }

  if (queue_empty(queue)) { return 0; }

  return queue->_end > queue->_start ? queue->_end - queue->_start : queue->_capacity - (queue->_start - queue->_end);
}

bool queue_empty(struct queue const *queue) {
  if (!queue || !queue->_data) return true;

  return queue->_empty;
}

static enum ds_error queue_resize_internal(struct queue *queue) {
  if (!queue) { return DS_ERROR; }

  if (queue->_capacity > (SIZE_MAX >> GROWTH_FACTOR) / queue->_data_size) { return DS_NO_MEM; }

  size_t old_capacity = queue->_capacity;
  queue->_capacity = queue->_capacity << GROWTH_FACTOR;

  void *tmp = realloc(queue->_data, queue->_data_size * queue->_capacity);
  if (!tmp) { return DS_NO_MEM; }

  if (queue->_end > queue->_start) { goto resize_end; }

  // no micro-optimizations. if queue:_end < queue::_start - queue::_start along with all element up to old_capacity
  // will be moved. potentially costly of situaltions where queue::_start & queue::_end are at a lower index
  size_t spacing = old_capacity - queue->_start;

  (void)memmove((char *)tmp + (queue->_capacity - spacing) * queue->_data_size,
                (char *)tmp + queue->_start * queue->_data_size,
                spacing * queue->_data_size);

  queue->_start = queue->_capacity - spacing;

resize_end:
  queue->_data = tmp;
  return DS_OK;
}

enum ds_error queue_enqueue(struct queue *restrict queue, void const *restrict element) {
  if (!queue || !queue->_data) { return DS_ERROR; }

  if (!element) { return DS_ERROR; }

  if (!queue->_empty && queue->_start == queue->_end) {
    enum ds_error ret = queue_resize_internal(queue);

    if (ret != DS_OK) { return ret; }
  }

  if (!memcpy((char *)queue->_data + queue->_end * queue->_data_size, element, queue->_data_size)) { return DS_ERROR; }

  queue->_end = (queue->_end + 1) % queue->_capacity;
  queue->_empty = false;

  return DS_OK;
}

enum ds_error queue_dequeue(struct queue *restrict queue, void *restrict element) {
  if (!queue || !queue->_data) { return DS_ERROR; }

  if (!element) { return DS_ERROR; }

  if (queue_empty(queue)) { return DS_NOT_FOUND; }

  if (!memcpy(element, (char *)queue->_data + queue->_start * queue->_data_size, queue->_data_size)) {
    return DS_ERROR;
  }

  queue->_start = (queue->_start + 1) % queue->_capacity;

  if (queue->_start == queue->_end) { queue->_empty = true; }

  return DS_VALUE_OK;
}

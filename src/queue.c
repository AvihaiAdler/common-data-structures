#include "queue.h"
#include <stdlib.h>

#define QUEUE_INIT_CAPACITY 64

struct queue queue_create(size_t data_size, void (*destroy)(void *_element)) {
  if (!data_size) { goto empty_queue_lbl; }

  void *data = malloc(data_size * QUEUE_INIT_CAPACITY);
  if (!data_size) { goto empty_queue_lbl; }

  return (struct queue){._capacity = QUEUE_INIT_CAPACITY, ._destroy = destroy, ._data_size = data_size, ._data = data};

empty_queue_lbl:
  return (struct queue){0};
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
  if (!queue || !queue->_elements_count) { return 0; }

  return queue->_end > queue->_start ? queue->_end - queue->_start : queue->_capacity - queue->_start - queue->_end;
}

bool queue_empty(struct queue const *queue) {
  if (!queue || !queue->_data) return true;

  return queue->_end == queue->_start;
}

bool queue_enqueue(struct queue *restrict queue, void const *restrict element);

enum ds_error queue_dequeue(struct queue *restrict queue, void *restrict element);
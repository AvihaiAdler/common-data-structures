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

struct queue queue_create(size_t data_size, void (*destroy)(void *_element));

void queue_destroy(struct queue *queue);

size_t queue_capacity(struct queue const *queue);

size_t queue_size(struct queue const *queue);

bool queue_empty(struct queue const *queue);

enum ds_error queue_enqueue(struct queue *restrict queue, void const *restrict element);

enum ds_error queue_dequeue(struct queue *restrict queue, void *restrict element);

#include <assert.h>
#include "queue.h"

static void queue_create_test(void) {
  struct queue queue = queue_create(sizeof(size_t), NULL);

  assert(queue_size(&queue) == 0);
  assert(queue_capacity(&queue) > 0);
  assert(queue_empty(&queue));

  queue_destroy(&queue);
}

static void queue_enqueue_small_test(int *arr, size_t size) {
  struct queue queue = queue_create(sizeof *arr, NULL);

  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_enqueue(&queue, &arr[i]);
    assert(ret == DS_OK);
  }

  size_t ret_size = queue_size(&queue);
  assert(ret_size == size);
  assert(!queue_empty(&queue));

  queue_destroy(&queue);
}

static void queue_enqueue_large_test(int *arr, size_t size) {
  struct queue queue = queue_create(sizeof *arr, NULL);

  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_enqueue(&queue, &arr[i]);
    assert(ret == DS_OK);
  }

  assert(queue_size(&queue) == size);
  assert(!queue_empty(&queue));

  queue_destroy(&queue);
}

static void queue_dequeue_small_test(int *arr, size_t size) {
  struct queue queue = queue_create(sizeof *arr, NULL);

  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_enqueue(&queue, &arr[i]);
    assert(ret == DS_OK);
  }

  int element;
  for (size_t i = 0; i < size / 16; i++) {
    enum ds_error ret = queue_dequeue(&queue, &element);

    assert(ret == DS_VALUE_OK);
  }

  assert(queue_size(&queue) == size - size / 16);
  assert(!queue_empty(&queue));

  queue_destroy(&queue);
}

static void queue_dequeue_all_test(int *arr, size_t size) {
  struct queue queue = queue_create(sizeof *arr, NULL);

  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_enqueue(&queue, &arr[i]);
    assert(ret == DS_OK);
  }

  int element;
  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_dequeue(&queue, &element);

    assert(ret == DS_VALUE_OK);
  }

  assert(queue_size(&queue) == 0);
  assert(queue_empty(&queue));

  queue_destroy(&queue);
}

static void queue_dequeue_empty_test(void) {
  struct queue queue = queue_create(sizeof(size_t), NULL);

  assert(queue_size(&queue) == 0);
  assert(queue_empty(&queue));

  size_t element;
  enum ds_error ret = queue_dequeue(&queue, &element);

  assert(ret == DS_NOT_FOUND);

  queue_destroy(&queue);
}

static void enqueue_some(struct queue *restrict queue, int *restrict arr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_enqueue(queue, &arr[i]);
    assert(ret == DS_OK);
  }
}

static void dequeue_some(struct queue *restrict queue, size_t size) {
  int element;
  for (size_t i = 0; i < size; i++) {
    enum ds_error ret = queue_dequeue(queue, &element);

    assert(ret == DS_VALUE_OK);
  }
}

static void queue_enqueue_dequeue(int *arr, size_t size) {
  struct queue queue = queue_create(sizeof *arr, NULL);

  size_t portion_size = size / 16;
  for (size_t i = 0; i < size; i += portion_size) {
    enqueue_some(&queue, arr + i, portion_size);
    dequeue_some(&queue, portion_size);
  }

  queue_destroy(&queue);
}

static void queue_enqueue_dequeue_portion(int *arr, size_t size) {
  struct queue queue = queue_create(sizeof *arr, NULL);

  size_t portion_size = size / 16;
  for (size_t i = 0; i < size; i += portion_size) {
    enqueue_some(&queue, arr, portion_size);
    dequeue_some(&queue, portion_size - 1);
  }

  queue_destroy(&queue);
}

static void arr_fill(int *arr, size_t size) {
  for (size_t i = 0; i < size; i++) { arr[i] = i + 1; }
}

int main(void) {
  enum {
    SIZE = 128,
  };

  int arr[SIZE] = {0};
  size_t size = sizeof arr / sizeof *arr;

  arr_fill(arr, size);

  queue_create_test();
  queue_enqueue_small_test(arr, size / 16);
  queue_enqueue_large_test(arr, size);
  queue_dequeue_small_test(arr, size);
  queue_dequeue_all_test(arr, size);
  queue_dequeue_empty_test();

  queue_enqueue_dequeue(arr, size);
  queue_enqueue_dequeue_portion(arr, size);
}

#include "src/vector_abstract.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vect *vect_init(size_t data_size) {
  // data_size validation check
  if (data_size == 0) {
    errno_t = INVALID_VAL;
    return NULL;
  }

  // vect allocation
  struct vect *vect = calloc(1, sizeof *vect);
  if (!vect) {
    errno_t = MEM_ALLOCATION_FAILURE;
    return NULL;
  }

  // vect initialization
  vect->capacity = INIT_CAPACITY;
  vect->data_size = data_size;

  // data initialization
  vect->data = calloc(INIT_CAPACITY, data_size);
  if (!vect->data) {
    errno_t = MEM_ALLOCATION_FAILURE;
    vect_destroy(vect);
    return NULL;
  }
  return vect;
}

void vect_destroy(void *vect) {
  struct vect *vector = vect;
  if (vector) {
    if (vector->data) free(vector->data);
    free(vector);
  }
}

void *vect_data(void *vect) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return NULL;
  }

  struct vect *vector = vect;
  return vector->data;
}

bool vect_empty(void *vect) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return false;
  }

  struct vect *vector = vect;
  return vector->size == 0;
}

size_t vect_size(void *vect) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return 0;
  }

  struct vect *vector = vect;
  return vector->size;
}

size_t vect_capacity(void *vect) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return 0;
  }

  struct vect *vector = vect;
  return vector->capacity;
}

err_t vect_reserve(void *vect, unsigned int num_of_elements) {
  if (!vect) {
    return INVALID_VAL;
  }

  struct vect *vector = vect;
  if (num_of_elements > vector->capacity) {
    void *tmp = realloc(vector->data, num_of_elements * vector->data_size);
    if (!tmp) {
      return MEM_ALLOCATION_FAILURE;
    }
    vector->capacity = num_of_elements;
    vector->data = tmp;
  }
  return NO_ERR;
}

err_t vect_resize(void *vect) {
  if (!vect) {
    return INVALID_VAL;
  }

  struct vect *vector = vect;
  void *tmp = realloc(vector->data,
                      vector->capacity * GROWTH_FACTOR * vector->data_size);
  if (!tmp) {
    return MEM_ALLOCATION_FAILURE;
  }
  vector->capacity *= GROWTH_FACTOR;
  vector->data = tmp;

  return NO_ERR;
}

err_t vect_shrink_to_fit(void *vect) {
  if (!vect) {
    return INVALID_VAL;
  }

  struct vect *vector = vect;
  if (vector->size < vector->capacity) {
    void *tmp = realloc(vector->data, vector->size * vector->data_size);
    if (!tmp) {
      return MEM_ALLOCATION_FAILURE;
    }
    vector->capacity = vector->size;
    vector->data = tmp;
  }
  return NO_ERR;
}

unsigned int vect_at(void *vect, unsigned int index) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return 0;
  }

  struct vect *vector = vect;
  if (index >= vector->size) {
    errno_t = OUT_OF_BOUNDS;
    return 0;
  }

  errno_t = NO_ERR;
  return index;
}

unsigned int vect_push(void *vect) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return 0;
  }

  struct vect *vector = vect;
  if (vector->size == vector->capacity) {
    vect_resize(vect);
  }
  return (vector->size)++;
}

unsigned int vect_pop(void *vect) {
  if (!vect) {
    errno_t = INVALID_VAL;
    return 0;
  }

  struct vect *vector = vect;
  if (vect_empty(vect)) {
    errno_t = INVALID_VAL;
    return vector->size;
  }
  return (vector->size)--;
}

err_t vect_remove(void *vect, unsigned int index) {
  bool ret;
  unsigned int pos = vect_at(vect, index);
  if(errno_t != NO_ERR) {
    return errno_t;
  }

  if (!ret) {
    return INVALID_VAL;
  }

  struct vect *vector = vect;
  void *after_index = vector->data + vector->data_size * (index + 1);
  void *dest = vector->data + vector->data_size * index;
  memmove(dest, after_index, after_index - dest - 1);
  vector->size--;
  return NO_ERR;
}

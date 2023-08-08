#pragma once

#define GROWTH_FACTOR 1
#define VECT_INIT_CAPACITY 16

#define DS_EINVAL ((size_t)(~0))

enum ds_error {
  DS_OK,
  DS_ERROR = -1,
};

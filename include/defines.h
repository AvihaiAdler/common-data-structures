#pragma once

#define GROWTH_FACTOR 1

enum ds_error {
  DS_OK,
  DS_VALUE_OK,
  DS_ERROR = -1,
  DS_NO_MEM = -2,
  DS_NOT_FOUND = -3,
  DS_OUT_OF_BOUNDS = -4,
};

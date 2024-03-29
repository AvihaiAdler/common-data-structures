#pragma once

#include <stdbool.h>
#include <stddef.h>

enum ascii_str_codes {
  STR_C_STR = -1,
  STR_NEXISTS = -1,
};

struct ascii_str_long {
  size_t size;
  size_t capacity;
  char *data;
};

struct ascii_str_short {
  // room for 23 chars the 24th byte represent the number of available chars left
  char data[sizeof(struct ascii_str_long)];
};

struct ascii_str {
  union {
    struct ascii_str_long long_;
    struct ascii_str_short short_;
  };
  bool is_sso;

  // add padding for systems with alignment requirement of 8. assumes `sizeof(bool) != sizeof(void *)`. ever
  char padding_[sizeof(void *) - sizeof(bool)];
};

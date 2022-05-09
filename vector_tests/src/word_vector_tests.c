#include "word_vector_tests.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

struct word {
  char *word;
};

GENERATE_VECT(struct word, word)

static char *words[] = {"hello", "there", "world"};

void destroy_word(struct word word) {
  if (word.word) free(word.word);
}

void word_vect_push_test(void) {
  // given
  struct word_vector *words_vect = word_vect_init();

  // when
  size_t size = sizeof words / sizeof *words;
  for (size_t i = 0; i < size; i++) {
    size_t len = strlen(words[i]);

    char *w = calloc(1, len + 1);
    if (!w) {
      fprintf(stderr, "mem allocation failure for word: %s\n", words[i]);
      word_vect_destroy(words_vect, destroy_word);
      return;
    }

    strncpy(w, words[i], len);
    word_vect_push(words_vect, (struct word){.word = w});
  }

  // then
  assert(words_vect->size == size);
  struct word_wrapper *data = words_vect->data;
  for (size_t i = 0; i < size; i++) {
    assert(strcmp(words[i], data[i].value.word) == 0);
  }

  word_vect_destroy(words_vect, destroy_word);
}
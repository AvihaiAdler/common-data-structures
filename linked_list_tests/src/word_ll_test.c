#include "word_ll_test.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

struct word {
  char *word;
};

GENERATE_LINKED_LIST(struct word, word)

static char *words[] = {"simple", "test", "with", "heap", "allocation"};

static void destroy_word(struct word word) {
  if (word.word) free(word.word);
}

void word_ll_prepend_test(void) {
  // given
  struct word_linked_list *ll = word_ll_init();

  // when
  size_t size = sizeof words / sizeof *words;
  for (size_t i = 0; i < size; i++) {
    size_t word_len = strlen(words[i]);

    char *w = calloc(1, word_len + 1);
    if (!w) {
      fprintf(stderr, "mem allocation failure for word: %s\n", words[i]);
      word_ll_destroy(ll, destroy_word);
    }

    word_ll_prepend(ll, (struct word){.word = w});
  }

  // then
  assert(ll->size == size);
  word_ll_destroy(ll, destroy_word);
}
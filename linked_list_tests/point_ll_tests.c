#include "point_ll_tests.h"

#include <assert.h>

#include "linked_list.h"

struct point {
  int x, y;
};

static const struct point points[] = {{.x = 0, .y = 0},
                                      {.x = 1, .y = 1},
                                      {.x = 2, .y = 1},
                                      {.x = 4, .y = 2},
                                      {.x = 5, .y = 6}};

GENERATE_LINKED_LIST(struct point, point)

static struct point_linked_list *before(void) {
  struct point_linked_list *ll = point_ll_init();
  point_ll_append(ll, (struct point){.x = 0, .y = 0});
  point_ll_append(ll, (struct point){.x = 1, .y = 1});
  point_ll_append(ll, (struct point){.x = 2, .y = 1});
  point_ll_append(ll, (struct point){.x = 4, .y = 2});
  point_ll_append(ll, (struct point){.x = 5, .y = 6});
  return ll;
}

static void after(struct point_linked_list *ll) { point_ll_destroy(ll, NULL); }

bool equal_to(struct point a, struct point b) {
  return a.x == b.x && a.y == b.y;
}

void point_ll_prepend_test(void) {
  // given
  struct point_linked_list *ll = before();

  // when
  point_ll_prepend(ll, (struct point){.x = -1, .y = -1});

  // then
  assert(equal_to(ll->head->data, (struct point){.x = -1, .y = -1}));
  assert(equal_to(ll->head->next->data, points[0]));

  after(ll);
}

void point_ll_append_test(void) {
  // given
  // when
  struct point_linked_list *ll = before();

  // then
  struct point_node *data = ll->head;
  for (size_t i = 0; i < sizeof points / sizeof *points && data->next;
       i++, data = data->next) {
    assert(equal_to(data->data, points[i]));
  }

  after(ll);
}

// void point_ll_insert_higher_test(void);

// void point_ll_replace_test(void);

// void point_ll_remove_test(void);
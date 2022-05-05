#include "point_ll_tests.h"

#include <assert.h>
#include <stdio.h>

#include "linked_list.h"

struct point {
  int x, y;
};

static const struct point points[] = {{.x = 0, .y = 0},
                                      {.x = 7, .y = 1},
                                      {.x = 2, .y = 1},
                                      {.x = 4, .y = 2},
                                      {.x = 5, .y = 6}};

GENERATE_LINKED_LIST(struct point, point)

static struct point_linked_list *before(void) {
  struct point_linked_list *ll = point_ll_init();

  for (size_t i = 0; i < sizeof points / sizeof *points; i++) {
    point_ll_append(ll, points[i]);
  }

  return ll;
}

static void after(struct point_linked_list *ll) { point_ll_destroy(ll, NULL); }

bool equal_to(struct point a, struct point b) {
  return a.x == b.x && a.y == b.y;
}

// compare the structs based on point::x
int comparator(struct point a, struct point b) {
  return (a.x > b.x) - (a.x < b.x);
}

static void print_ll(struct point_linked_list *ll) {
  printf("size: %zu\n", ll->size);

  for (struct point_node *head = ll->head; head; head = head->next) {
    printf("{ x = %d, y = %d }\n", head->data.x, head->data.y);
  }
}

void point_ll_prepend_test(void) {
  // given
  struct point_linked_list *ll = point_ll_init();

  // when
  for (size_t i = 0; i < sizeof points / sizeof *points; i++) {
    point_ll_prepend(ll, points[i]);
  }

  // then
  struct point_node *data = ll->head;
  size_t size = sizeof points / sizeof *points;
  for (size_t i = 0; i < size && data->next; i++, data = data->next) {
    assert(equal_to(data->data, points[size - i - 1]));
  }

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

void point_ll_insert_higher_test(void) {
  // given
  struct point_linked_list *ll = point_ll_init();

  // when
  size_t size = sizeof points / sizeof *points;
  for (size_t i = 0; i < size; i++) {
    point_ll_insert_higher(ll, points[i], comparator);
  }
  // and
  struct point p = {.x = -1, .y = 0};
  point_ll_insert_higher(ll, p, comparator);

  // then
  assert(equal_to(ll->head->data, points[1]));
  assert(equal_to(ll->tail->prev->data, points[0]));
  assert(equal_to(ll->tail->data, p));

  after(ll);
}

// void point_ll_remove_first_test(void);

// void point_ll_remove_last_test(void);

// void point_ll_replace_test(void);

// void point_ll_remove_test(void);
#include "point_ll_tests.h"

#include <assert.h>

#include "linked_list.h"

struct point {
  int x, y;
};

GENERATE_LINKED_LIST(struct point, point);

static struct point_linked_list *before(void) {}

static void after(struct point_linked_list *ll) {}
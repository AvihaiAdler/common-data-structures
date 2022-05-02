#ifndef VECTOR_ABSTRACT_H_
#define VECTOR_ABSTRACT_H_

#include <stdbool.h>
#include <stddef.h>

#define INIT_CAPACITY 10
#define GROWTH_FACTOR 2

typedef enum {
  NO_ERR = 0,
  OUT_OF_BOUNDS = 1,
  MEM_ALLOCATION_FAILURE = 2,
  INVALID_VAL = 3,
} err_t;

static err_t errno_t = NO_ERR;

/* initialize a vector with initial size of 10 elements of size data_size. on
 * success returns a pointer to a heap allocated vect. on failure returns NULL
 * and set errno_t to a value != 0
 */
struct vect *vect_init(size_t data_size);

/* free all memory used by the vector */
void vect_destroy(void *vect);

/* returns the undelying array of the vector, set errno_t if vect is NULL */
void *vect_data(void *vect);

/* returns true if the vector is empty. false otherwise */
bool vect_empty(void *vect);

/* returns the number of elements in the vector */
size_t vect_size(void *vect);

/* returns the number of elements that can be held in the currently allocated
 * storage */
size_t vect_capacity(void *vect);

/* reserves storage */
err_t vect_reserve(void *vect, unsigned int num_of_elements);

/* changes the number of elements stored */
err_t vect_resize(void *vect);

/* reduces memory usage by freeing unused memory */
err_t vect_shrink_to_fit(void *vect);

/* access specified element with bounds checking. if index is out of bounds set
 * ret to false */
unsigned int vect_at(void *vect, unsigned int index);

/* returns the index for an element to be pushed. allocte memory if needed */
unsigned int vect_push(void *vect);

/* returns the index for a removed element */
unsigned int vect_pop(void *vect);

/* remove an element at index */
err_t vect_remove(void *vect, unsigned int index);

struct vect {
  size_t size;
  size_t capacity;
  size_t data_size;
  void *data;
};

#endif
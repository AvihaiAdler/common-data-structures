#include <include/vector.h>
#include <stdio.h>

GENERATE_VECT(int, int)
GENERATE_VECT(double, double)

int int_asc_comparator(const void* a, const void* b) {
  int num_a = *(int*)a;
  int num_b = *(int*)b;
  return (num_a > num_b) - (num_a < num_b);
}

int double_desc_comparator(const void* a, const void* b) {
  double d_a = *(double*)a;
  double d_b = *(double*)b;
  return (d_b > d_a) - (d_b < d_a);
}

int main(void) {
  struct Vector_int* vect_int = vect_int_init();
  for(int i = 9; i > 0; i--) {
    vect_int_push(vect_int, i);
  }

  printf("size: %zu\n", vect_size(vect_int));
  int* data = vect_int_data(vect_int);
  size_t size = vect_size(vect_int);
  for(size_t i = 0; i < size; i++) {
    printf("%d, ", data[i]);
  }
  printf("\b\b \n");

  vect_int_sort(vect_int, int_asc_comparator);

  printf("sorted asc: ");
  data = vect_int_data(vect_int);
  for(size_t i = 0; i < size; i++) {
    printf("%d, ", data[i]);
  }
  printf("\b\b \n");

  vect_destroy(vect_int);

  struct Vector_double* vect_double = vect_double_init();
  for(double i = 0.1; i < 2; i+= 0.1) {
    vect_double_push(vect_double, i);
  }

  double* double_data = vect_double_data(vect_double);
  size = vect_size(vect_double);
  for(size_t i = 0; i < size; i++) {
    printf("%.1lf, ", double_data[i]);
  }
  printf("\b\b \n");

  vect_double_sort(vect_double, double_desc_comparator);

  printf("sorted desc: ");
  for(size_t i = 0; i < size; i++) {
    printf("%.1lf, ", double_data[i]);
  }
  printf("\b\b \n");

  vect_destroy(vect_double);
  return 0;
}
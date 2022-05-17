#pragma once
#include <stddef.h>

#include "vector.h"

#define LOAD_FACTOR 0.7
#define TABLE_GROWTH 1
#define TABLE_INIT_CAPACITY 32

/* 'bucket' */
struct node {
  unsigned char *key;
  unsigned char *value;

  struct node *next;
  struct node *prev;
};

/* entry */
struct entry {
  unsigned long long size;  // number of 'buckets'
  struct node *head;
  struct node *tail;
  struct node *tmp;  // used internally for the rehash process
};

/* hash table struct */
struct hash_table {
  unsigned long long num_of_entries;
  unsigned long long num_of_elements;

  unsigned long long key_size;
  unsigned long long value_size;

  struct vector *entries;
};

/* creates a hash table with initial capacity of INIT_CAPACITY. key_size and
 * value_size are the size of the key && value in bytes. note that if you key or
 * value are of unknown / changing size (e.g. strings) you'll have to pass a
 * pointer to the data (i.e. for a string - a pointer to the string, char **,
 * etc) into the table instead and define key_size / value_size accordingly.
 * returns a pointer to a heap allocated table on success, NULL on failure */
struct hash_table *init_table(unsigned long long key_size,
                              unsigned long long value_size);

/* destroys the hash table. expects a destroy function (which may by NULL). if
 * it doesn't NULL calls it for evey key-value pair in the table. you should
 * only pass in a destroy function if you key or value contains a pointer to a
 * heap allocated memory */
void table_destroy(struct hash_table *table,
                   void (*destroy)(void *key, void *value));

/* returns true if the table is empty. false otherwise */
bool table_empty(struct hash_table *table);

/* returns the number of elements in the table */
unsigned long long table_size(struct hash_table *table);

/* creates a copy of the data passed in - and put it in the table. returns the
 * previous value for that key (which has to be free'd) or NULL if there was no
 * mapping for that key */
void *table_put(struct hash_table *table, const void *key, const void *value);

/* removes the mapping for a specific key if present. returns the previous value
 * (which has to be free'd) or NULL if there was no mapping for that key */
void *table_remove(struct hash_table *table, const void *key);

/* returns the mapping for a specific key if present or NULL if there was no
 * mapping for that key. the value must not be free'd */
void *table_get(struct hash_table *table, const void *key);

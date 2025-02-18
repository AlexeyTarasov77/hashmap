#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>

#define INITIAL_CAPACITY 10
#define HASHMAP_EMPTY_VALUE 0

typedef struct {
  char *key;
  void *value;
} linked_list_item_data;

typedef struct linked_list_item {
  linked_list_item_data data;
  struct linked_list_item *next;
} linked_list_item;

typedef struct {
  linked_list_item **arr;
  size_t size;
  size_t capacity;
} HashMap;

HashMap *create_hashmap();

void hashmap_add(HashMap *hashmap, char *key, void *value);
void *hashmap_get(HashMap *hashmap, char *key);

#endif

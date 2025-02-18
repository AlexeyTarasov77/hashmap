#include "hashmap.h"
#include <stdio.h>
#include <string.h>

#define HASH_SEED 0x000031FF

HashMap *create_hashmap() {
  HashMap *hashmap = malloc(sizeof(HashMap));
  hashmap->arr = calloc(INITIAL_CAPACITY, sizeof(linked_list_item *));
  hashmap->size = 0;
  hashmap->capacity = INITIAL_CAPACITY;
  return hashmap;
}

// external MurmurHash3 hash function
static uint32_t hashfunc(HashMap *hashmap, const char *key) {
  uint32_t h = HASH_SEED;
  size_t len = strlen(key);
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  for (uint32_t i = 0; i < len; i++) {
    uint32_t k = key[i];
    k *= c1;
    k = (k << 15) | (k >> 17);
    k *= c2;

    h ^= k;
    h = (h << 13) | (h >> 19);
    h = h * 5 + 0xe6546b64;
  }

  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h % hashmap->capacity;
}

static void hashmap_extend(HashMap *hashmap) {
  // reallocate data and reapply hashfunc during copying
  size_t new_capacity =
      hashmap->capacity * 2 * sizeof(linked_list_item *); // x2 capacity
  linked_list_item **dst = malloc(new_capacity);
  memcpy(dst, hashmap->arr, hashmap->capacity);
  free(hashmap->arr);
  hashmap->arr = dst;
  hashmap->capacity = new_capacity;
}

void hashmap_add(HashMap *hashmap, char *key, void *value) {
  float a = (float)hashmap->size / (float)hashmap->capacity;
  if (a >= 0.8) {
    hashmap_extend(hashmap);
  }
  uint32_t i = hashfunc(hashmap, key);
  linked_list_item *new_item = malloc(sizeof(linked_list_item));
  new_item->data.key = key;
  new_item->data.value = value;
  if (hashmap->arr[i] != HASHMAP_EMPTY_VALUE) {
    if (hashmap->arr[i]->data.key == key) {
      hashmap->arr[i]->data.value = value;
      free(new_item);
      return;
    }
    // Resolving collision by adding item to a linked list
    linked_list_item *next = hashmap->arr[i]->next;
    if (next == NULL) {
      hashmap->arr[i]->next = new_item;
      return;
    }
    while (next->next != NULL) {
      next = next->next;
    }
    next->next = new_item;
  } else {
    hashmap->arr[i] = new_item;
  }
}

void *hashmap_get(HashMap *hashmap, char *key) {
  uint32_t i = hashfunc(hashmap, key);
  linked_list_item *next = hashmap->arr[i];
  while (next != NULL) {
    if (strlen(next->data.key) == strlen(key) &&
        strcmp(next->data.key, key) == 0) {
      return next->data.value;
    }
    next = next->next;
  }
  return NULL;
}

void run_tests() {
  puts("Running tests...");
  HashMap *hashmap = create_hashmap();
  hashmap_add(hashmap, "a", &(int){1});
  hashmap_add(hashmap, "b", &(int){3});
  hashmap_add(hashmap, "c", &(int){10});
  hashmap_add(hashmap, "str", "hello");
  hashmap_add(hashmap, "pi", &(float){3.14});
  uint8_t is_failed = 0;
  void *tmp;
  if ((tmp = hashmap_get(hashmap, "a")) == NULL || *(int *)tmp != 1) {
    puts("Test 1 failed!");
    is_failed = 1;
  }
  if ((tmp = hashmap_get(hashmap, "b")) == NULL || *(int *)tmp != 3) {
    puts("Test 2 failed!");
    is_failed = 1;
  }
  if ((tmp = hashmap_get(hashmap, "c")) == NULL || *(int *)tmp != 10) {
    puts("Test 3 failed!");
    is_failed = 1;
  }
  if ((tmp = hashmap_get(hashmap, "str")) == NULL ||
      strcmp((char *)tmp, "hello") != 0) {
    puts("Test 4 failed!");
    is_failed = 1;
  }
  if ((tmp = hashmap_get(hashmap, "pi")) == NULL || *(float *)tmp != 3.14F) {
    puts("Test 5 failed!");
    is_failed = 1;
  }

  free(hashmap->arr);
  free(hashmap);
  if (!is_failed)
    puts("All tests succesfully passed\n");
}

int main(int argc, char *argv[]) {
  if (argv[1] != NULL && strncmp(argv[1], "test", 4) == 0)
    run_tests();
}

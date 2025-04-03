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

void free_hashmap(HashMap *hashmap) {
  for (int i = 0; i < hashmap->capacity; i++) {
    free(hashmap->arr[i]);
  }
  free(hashmap->arr);
  free(hashmap);
}

// external MurmurHash3 hash function
static uint32_t hash(HashMap *hashmap, const char *key) {
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
  printf("Extending\n");
  // reallocate data and reapply hashfunc during copying
  size_t old_capacity = hashmap->capacity;
  size_t new_capacity = old_capacity * 2; // x2 capacity
  linked_list_item **dst = calloc(new_capacity, sizeof(linked_list_item *));
  hashmap->capacity = new_capacity;
  for (int i = 0; i < old_capacity; i++) {
    linked_list_item *item = hashmap->arr[i];
    if (item == HASHMAP_EMPTY_VALUE)
      continue;
    // recompute hash to distribute elements
    // accross new available space
    uint32_t j = hash(hashmap, item->data.key);
    dst[j] = item;
    if (item->next == NULL)
      continue;
    linked_list_item *prev = item;
    linked_list_item *next = item->next;
    while (next != NULL) {
      uint32_t k = hash(hashmap, next->data.key);
      if (k != j) {
        dst[k] = next;
        next = next->next;
        prev->next = NULL;
      } else {
        prev->next = next;
        prev = next;
        next = next->next;
      }
    }
  }
  free(hashmap->arr);
  hashmap->arr = dst;
}

void hashmap_add(HashMap *hashmap, char *key, void *value) {
  float fill_factor = (float)hashmap->size / (float)hashmap->capacity;
  if (fill_factor >= 0.7) {
    hashmap_extend(hashmap);
  }
  uint32_t i = hash(hashmap, key);
  printf("Index for %s: %d\n", key, i);
  linked_list_item *new_item = malloc(sizeof(linked_list_item));
  new_item->data.key = key;
  new_item->data.value = value;
  if (hashmap->arr[i] != HASHMAP_EMPTY_VALUE) {
    printf("Comparing %s to %s\n", hashmap->arr[i]->data.key, key);
    if (strcmp(hashmap->arr[i]->data.key, key) == 0) {
      printf("Equal! UPdating value\n");
      hashmap->arr[i]->data.value = value;
      free(new_item);
      return;
    }
    hashmap->size++;
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
    return;
  }
  hashmap->size++;
  hashmap->arr[i] = new_item;
}

int hashmap_remove(HashMap *hashmap, char *key) {
  uint32_t i = hash(hashmap, key);
  linked_list_item *next = hashmap->arr[i];
  if (next == NULL || next->next == NULL) {
    hashmap->arr[i] = NULL;
    return 0;
  }
  linked_list_item *prev = next;
  next = next->next;
  while (next != NULL) {
    if (strlen(next->data.key) == strlen(key) &&
        strcmp(next->data.key, key) == 0) {
      prev->next = NULL;
      return 0;
    }
    prev = next;
    next = next->next;
  }
  return 1;
}

void *hashmap_get(HashMap *hashmap, char *key) {
  uint32_t i = hash(hashmap, key);
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
  const uint8_t count_all_tests = 8;
  uint8_t count_tests_failed = 0;
  void *tmp;
  char test_key[] = "string";
  char test_dup_key[sizeof(test_key)];
  strcpy(test_dup_key, test_key);
  int numbers[] = {1, 2, 3};
  HashMap *hashmap = create_hashmap();
  hashmap_add(hashmap, "a", &(int){1});
  hashmap_add(hashmap, "b", &(int){3});
  hashmap_add(hashmap, "c", &(int){10});
  hashmap_add(hashmap, "d", &(int){10});
  if (hashmap_remove(hashmap, "d") != 0) {
    puts("Hashmap delete failed");
    count_tests_failed++;
  }
  hashmap_add(hashmap, "pi", &(float){3.14F});
  hashmap_add(hashmap, "name", "steven");
  hashmap_add(hashmap, "numbers", numbers);
  hashmap_add(hashmap, test_key, "hello");
  hashmap_add(hashmap, test_dup_key, "test");
  if ((tmp = hashmap_get(hashmap, "a")) == NULL || *(int *)tmp != 1) {
    puts("Test 1 failed!");
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "b")) == NULL || *(int *)tmp != 3) {
    puts("Test 2 failed!");
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "c")) == NULL || *(int *)tmp != 10) {
    printf("Test 3 failed! Expected: %d Got: %p\n", 10, tmp);
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "d")) != NULL) {
    puts("Test 4 failed!");
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "pi")) == NULL || *(float *)tmp != 3.14F) {
    puts("Test 5 failed!");
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "name")) == NULL ||
      strcmp(tmp, "steven") != 0) {
    puts("Test 6 failed!");
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "numbers")) == NULL || *(int *)tmp != 1) {
    printf("Test 7 failed! Got: %p\n", tmp);
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, test_key)) == NULL ||
      strcmp(tmp, "test") != 0) {
    printf("Test 8 failed! Expected: %s Got: %s\n", "test", (char *)tmp);
    count_tests_failed++;
  }
  if (hashmap->capacity != INITIAL_CAPACITY * 2) {
    puts("Test 9 failed!");
    count_tests_failed++;
  }
  if (hashmap->size != 8) {
    printf("Test 10 failed! Expected: %d, got: %ld\n", 8, hashmap->size);
    count_tests_failed++;
  }
  free_hashmap(hashmap);
  if (count_tests_failed)
    printf("%d tests failed out of %d", count_tests_failed, count_all_tests);
  else
    puts("All tests succesfully passed");
}

int main(int argc, char *argv[]) {
  if (argv[1] != NULL && strncmp(argv[1], "test", 4) == 0)
    run_tests();
}

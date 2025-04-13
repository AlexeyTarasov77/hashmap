#include "hashmap.h"
#include <stdio.h>
#include <string.h>

void run_tests() {
  puts("Running tests...");
  const int count_all_tests = 8;
  int count_tests_failed = 0;
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
  if (hashmap_remove(hashmap, "d") != 0) {
    puts("Hashmap delete failed");
    count_tests_failed++;
  }
  if ((tmp = hashmap_get(hashmap, "d")) != NULL) {
    puts("Item exists after deletion");
    count_tests_failed++;
  }
  if (hashmap->size != 7) {
    puts("Deletion didn't decrement size");
    count_tests_failed++;
  }
  hashmap_item *items = hashmap_list_items(hashmap);
  for (int i = 0; i < hashmap->size; i++) {
    printf("%s: %p\n", items[i].key, items[i].value);
  }
  free_hashmap(hashmap);
  if (count_tests_failed)
    printf("%d tests failed out of %d", count_tests_failed, count_all_tests);
  else
    puts("All tests succesfully passed");
}

int main() { run_tests(); }

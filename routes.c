#include "routes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_SIZE 254

// TODO: Remove item
// TODO: Free table

unsigned int hash(char *path, int size) {
  int len = strnlen(path, PATH_SIZE);
  unsigned int result;

  for (int i = 0; i < len; i++) {
    result += path[i];
    result = (result * path[i]) % size;
  }

  return result;
}

route_table *new_table(int size) {
  route_table *table = malloc(size);
  table->size = size;
  table->items = calloc(size, sizeof(route_item));

  for (int i = 0; i < size; i++) {
    table->items[i] = NULL;
  }

  return table;
};

route_item *new_item(char *path, route_fn fn) {
  route_item *item = malloc(sizeof(route_item));
  item->path = malloc(strnlen(path, PATH_SIZE));
  strncpy(item->path, path, PATH_SIZE);
  item->fn = fn;

  return item;
};

void print_table(route_table *table) {
  printf("-----------------------\n");
  for (int i = 0; i < table->size; i++) {
    if (table->items[i]) {
      printf("%s\n", table->items[i]->path);
    } else {
      printf("--\n");
    }
  }
  printf("-----------------------\n");
}

int insert(route_table *table, route_item *item) {
  // TODO: Handle collisions
  int index = hash(item->path, table->size);
  table->items[index] = item;
  return 1;
}

route_item *get_item(char *path, route_table *table) {
  int index = hash(path, table->size);
  if (table->items[index] && strcmp(table->items[index]->path, path) == 0) {
    return table->items[index];
  }
  return NULL;
}

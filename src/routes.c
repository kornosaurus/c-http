#include "include/routes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_SIZE 254

// TODO: Remove item
// TODO: Free table
// TODO: Handle collisions

RouteTable *new_table(int size) {
  RouteTable *table = malloc(size);
  table->size = size;
  table->items = calloc(size, sizeof(RouteItem));

  for (int i = 0; i < size; i++) {
    table->items[i] = NULL;
  }

  return table;
};

RouteItem *new_item(char *path, RouteFn fn) {
  RouteItem *item = malloc(sizeof(RouteItem));
  item->path = malloc(strnlen(path, PATH_SIZE));
  strncpy(item->path, path, PATH_SIZE);
  item->fn = fn;

  return item;
};

void print_table(RouteTable *table) {
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

unsigned int hash(char *path, int size) {
  int len = strnlen(path, PATH_SIZE);
  unsigned int result;

  for (int i = 0; i < len; i++) {
    result += path[i];
    result = (result * path[i]) % size;
  }

  return result;
}

int insert(RouteTable *table, RouteItem *item) {
  // TODO: Handle collisions
  int index = hash(item->path, table->size);
  table->items[index] = item;
  return 1;
}

RouteItem *get_item(char *path, RouteTable *table) {
  int index = hash(path, table->size);
  if (table->items[index] && strcmp(table->items[index]->path, path) == 0) {
    return table->items[index];
  }
  return NULL;
}

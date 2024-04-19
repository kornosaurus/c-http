#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_SIZE 254

typedef int (*route_fn)(char **, char *);

typedef struct {
  enum HttpMethod method;
  char *path;
  route_fn fn;
} route_item;

typedef struct {
  route_item **items;
  int size;
} route_table;

unsigned int hash(route_item *item, int size) {
  int len = strnlen(item->path, PATH_SIZE);
  unsigned int result;

  for (int i = 0; i < len; i++) {
    result += item->path[i] * item->method;
    result = (result * item->path[i]) % size;
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

route_item *new_item(enum HttpMethod method, char *path, route_fn fn) {
  route_item *item = malloc(sizeof(route_item));
  item->path = malloc(strnlen(path, PATH_SIZE));
  strncpy(item->path, path, PATH_SIZE);
  item->method = method;
  item->fn = fn;

  return item;
};

void print_table(route_table *table) {
  printf("-----------------------\n");
  for (int i = 0; i < table->size; i++) {
    if (table->items[i]) {
      printf("%s\t%s\n", http_method_to_string(table->items[i]->method),
             table->items[i]->path);
    } else {
      printf("--\n");
    }
  }
  printf("-----------------------\n");
}

int insert(route_table *table, route_item *item) {
  // TODO: Handle collisions
  int index = hash(item, table->size);
  table->items[index] = item;
  return 1;
}

// TODO: Get item
// TODO: Remove item
// TODO: Free table

int main() {
  route_table *table = new_table(8);
  insert(table, new_item(GET, "/hello/world", NULL));
  insert(table, new_item(GET, "/foo/bar", NULL));
  insert(table, new_item(POST, "/foo/bar", NULL));
  insert(table, new_item(GET, "/bar", NULL));
  print_table(table);
}

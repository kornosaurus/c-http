#include "include/routes.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_SIZE 254

// TODO: Remove item
// TODO: Free table

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
      RouteItem *next = table->items[i]->next;
      while (next) {
          printf("%s\n", next->path);
          next = next->next;
      }
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
  int index = hash(item->path, table->size);

  if (table->items[index]) {
      RouteItem *slot = table->items[index];
      while(slot->next) {
          slot = slot->next;
      }
      if (strcmp(slot->path, item->path) != 0) {
        slot->next = item;
      }
  } else {
      table->items[index] = item;
  }
  return 1;
}

RouteItem *get_item(char *path, RouteTable *table) {
  int index = hash(path, table->size);
  if (table->items[index] && strcmp(table->items[index]->path, path) == 0) {
    return table->items[index];
  } else if (table->items[index]) {
    RouteItem *next  = table->items[index]->next;
    while(next) {
        if (strcmp(next->path, path) == 0) {
            return next;
        }
        next = next->next;
    }
  }
  return NULL;
}

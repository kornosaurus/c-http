#ifndef ROUTES_H_INCLUDED
#define ROUTES_H_INCLUDED

typedef int (*route_fn)(char **, char *);

typedef struct {
  char *path;
  route_fn fn;
} route_item;

typedef struct {
  route_item **items;
  int size;
} route_table;

route_table *new_table(int size);

route_item *new_item(char *path, route_fn fn);

void print_table(route_table *table);

int insert(route_table *table, route_item *item);

route_item *get_item(char *path, route_table *table);

#endif

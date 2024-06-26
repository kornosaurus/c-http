#ifndef ROUTES_H_INCLUDED
#define ROUTES_H_INCLUDED

typedef struct {
  int status_code;
  char *headers; // TODO Struct?
  char *data;
} Response;

typedef int *(*RouteFn)(Response *, char *);

typedef struct RouteItem {
  char *path;
  RouteFn fn;
  struct RouteItem *next;
} RouteItem;

typedef struct {
  RouteItem **items;
  int size;
} RouteTable;

RouteTable *new_table(int size);

RouteItem *new_item(char *path, RouteFn fn);

void print_table(RouteTable *table);

int insert(RouteTable *table, RouteItem *item);

RouteItem *get_item(char *path, RouteTable *table);

#endif

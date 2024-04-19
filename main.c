#include "server.h"

Response *test_route_fn(char *path) {
  Response *response;
  response->status_code = 200;
  response->headers = "Content-Type: text/plain";
  response->data = "suuuh dud";
  return response;
}

int main() {
  RouteTable *route_table = new_table(10);
  insert(route_table, new_item("/test/path", test_route_fn));

  start_server(route_table);
}

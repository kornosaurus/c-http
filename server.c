#include <arpa/inet.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 3000
#define BUFFER_SIZE 1024
#define PATH_SIZE 64

enum HttpMethod { NONE = 0, GET = 1, POST = 2 };

struct Route {
  enum HttpMethod method;
  char *path;
  void (*fn)(char *); // FIXME: Is this a good idea?
};

int find_route(struct Route *res, char *path, enum HttpMethod method,
               struct Route *routes, int size) {
  if (method == NONE) {
    return 0;
  }
  for (int i = 0; i < size; i++) {
    if (routes[i].method == method && strcmp(path, routes[i].path) == 0) {
      *res = routes[i];
      return 1;
    }
  }
  return 0;
}

enum HttpMethod map_http_method(char *method) {
  if (strcmp(method, "GET") == 0) {
    return GET;
  } else if (strcmp(method, "GET") == 0) {
    return POST;
  }
  return NONE;
}

void accept_connections(int sock, struct Route *routes, int size) {
  regmatch_t matches[3];
  regex_t res_regex;

  if (regcomp(&res_regex, "^([A-Z]+) ([^ ]+) HTTP/1", REG_EXTENDED) < 0) {
    printf("Failed to compile regex");
    exit(EXIT_FAILURE);
  }

  while (1) {
    int client_fd;
    char buffer[BUFFER_SIZE];
    char *path = malloc(PATH_SIZE);
    char *method;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    if ((client_fd = accept(sock, (struct sockaddr *)&client_addr,
                            &client_addr_len)) < 0) {
      printf("Accept failed");
      exit(EXIT_FAILURE);
    }

    int bytes_read = read(client_fd, buffer, sizeof(buffer));

    if (bytes_read > 0) {
      if (regexec(&res_regex, buffer, 3, matches, 0) < 0) {
        printf("Failed to exec regex on request");
        exit(EXIT_FAILURE);
      };

      buffer[matches[2].rm_eo] = 0;
      strcpy(path, buffer + matches[2].rm_so);

      buffer[matches[1].rm_eo] = 0;
      method = buffer + matches[1].rm_so;

      struct Route res;
      int success = find_route(&res, path, map_http_method(method), routes, 1);

      if (success > 0) {
        (*res.fn)(path);
      } else {
        printf("404: %s %s\n", method, path);
      }
    }

    fflush(stdout);
    close(client_fd);
    free(path);
  }
}

void test_route_fn(char *p) { printf("TEST ROUTE CALLED: %s\n", p); }

int main() {
  // TODO Implement a hashmap to store routes?
  int idx = 0;
  struct Route **routes = malloc(10);
  struct Route test_route;

  test_route.path = "/test/path";
  test_route.method = GET;
  test_route.fn = &test_route_fn;

  routes[idx++] = &test_route;

  // SOCKET
  struct sockaddr_in server_addr;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(sock, 10) < 0) {
    printf("listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Listening to port %d\n\n", PORT);
  fflush(stdout);
  // END SOCKET

  accept_connections(sock, *routes, idx);

  free(routes);
}

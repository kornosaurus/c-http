#include "include/routes.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// TODO Better logging

#define PORT 3000
#define BUFFER_SIZE 1024
#define PATH_SIZE 256

typedef struct {
  int *sock;
  int *client_fd;
  RouteTable *table;
} ClientCtx;

int create_response(Response *res, char *out, int size) {
  snprintf(out, size,
           "HTTP/1.1 %d OK\r\n"
           "%s\r\n"
           "\r\n"
           "%s",
           res->status_code, res->headers, res->data);
  return 1;
}

Response not_found_response = {.status_code = 404,
                               .headers = "Content-Type: text/plain",
                               .data = "Not found"};

void *handle_client(void *args) {
  regmatch_t matches[3];
  regex_t res_regex;

  if (regcomp(&res_regex, "^([A-Z]+) ([^ ]+) HTTP/1", REG_EXTENDED) < 0) {
    printf("Failed to compile regex");
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE];
  char *path = malloc(PATH_SIZE);
  char *method;
  ClientCtx *ctx = (ClientCtx *)args;
  int bytes_read = read(*ctx->client_fd, buffer, sizeof(buffer));

  if (bytes_read > 0) {
    if (regexec(&res_regex, buffer, 3, matches, 0) < 0) {
      perror("Failed to exec regex on request");
      exit(EXIT_FAILURE);
    };

    buffer[matches[2].rm_eo] = 0;
    strncpy(path, buffer + matches[2].rm_so, PATH_SIZE);

    buffer[matches[1].rm_eo] = 0;
    method = buffer + matches[1].rm_so;

    RouteItem *route = get_item(path, ctx->table);
    char *response_str = malloc(BUFFER_SIZE);

    if (route) {
      Response *response_obj = malloc(sizeof(Response));
      // TODO Handle return value from route fn
      (*route->fn)(response_obj, path);
      create_response(response_obj, response_str, BUFFER_SIZE);
      printf("%d %s %s\n", response_obj->status_code, method, path);
      free(response_obj);
    } else {
      create_response(&not_found_response, response_str, BUFFER_SIZE);
      printf("404 %s %s\n", method, path);
    }

    if (send(*ctx->client_fd, response_str, strlen(response_str), 0) < 0) {
      perror("Failed to send response");
    };

    free(response_str);
  }

  close(*ctx->client_fd);
  free(path);
  free(ctx);
  fflush(stdout);
  return NULL;
}

void accept_connections(int sock, RouteTable *table) {
  while (1) {
    int *client_fd = malloc(sizeof(int));
    pthread_t *thread = malloc(sizeof(pthread_t));
    ClientCtx *client_ctx = malloc(sizeof(ClientCtx));

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    if ((*client_fd = accept(sock, (struct sockaddr *)&client_addr,
                             &client_addr_len)) < 0) {
      perror("Connection failed");
      exit(EXIT_FAILURE);
    }

    client_ctx->client_fd = client_fd;
    client_ctx->sock = &sock;
    client_ctx->table = table;

    if (pthread_create(thread, NULL, handle_client, client_ctx) != 0) {
      perror("Could not start thread");
      exit(EXIT_FAILURE);
    }

    pthread_detach(*thread);
  }
}

void start_server(RouteTable *route_table) {
  struct sockaddr_in server_addr;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(sock, 10) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Listening to port %d\n\n", PORT);
  fflush(stdout);

  accept_connections(sock, route_table);
}

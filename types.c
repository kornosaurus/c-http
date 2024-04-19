#include "types.h"
#include <string.h>

enum HttpMethod http_method_from_string(char *method) {
  if (strcmp(method, "GET") == 0) {
    return GET;
  } else if (strcmp(method, "GET") == 0) {
    return POST;
  }
  return NONE;
}

char *http_method_to_string(enum HttpMethod method) {
  char *methods[3] = {"NONE", "GET", "POST"};
  return methods[method];
};

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

enum HttpMethod { NONE = 0, GET = 1, POST = 2 };

enum HttpMethod http_method_from_string(char *method);
char *http_method_to_string(enum HttpMethod method);

#endif

#ifndef HTTP_METHODS_H
#define HTTP_METHODS_H

typedef enum http_methods http_methods;

enum http_methods {
  OPTIONS,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT
};

#endif // HTTP_METHODS_H

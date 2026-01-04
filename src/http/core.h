#ifndef HTTP_CORE_H
#define HTTP_CORE_H

typedef enum http_codes http_codes;
typedef enum http_methods http_methods;

#ifndef HTTP_VERSION
#define HTTP VERSION 1.1
#endif

enum http_codes {
  OK = 200,
  NOT_IMPLEMENTED = 501,
};

enum http_methods {
  OPTIONS,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT,
  UNRECOGNIZED
};

#endif // HTTP_CORE_H

#ifndef CORE_H
#define CORE_H

typedef enum http_codes http_codes;
typedef enum http_methods http_methods;

#ifndef HTTP_VERSION
#define HTTP VERSION "HTTP/1.1" 
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

#endif // CORE_H

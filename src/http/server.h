#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

#include "../utils/thpool.h"

typedef struct http_server http_server;
struct http_server {
  int socket_fd; 
  struct addrinfo *serv_info;
  thpool thpool;
  char port[6];
  char ip_str[INET6_ADDRSTRLEN];
  int backlog;
};

/*
 * Initializes the server: creates, bind and listens
 * param: server: the server to be initialized
 * param: port: port to bind to
 * param: thr_count: number of threads for request processing
 * backlog: maximum number of pending connections                  
 * return: NULL on failure
 *         pointer to server on success
 * NOTE: must be deinitialized through a call to deinit_http_server
 */
http_server *init_http_server(
    http_server *server,
    char *port,
    int thr_count,
    int backlog
    );

/*
 * Deinitializes the server
 * param: server: server to be deinitialized
 * NOTE: safe to call with NULL ptr
 */
void deinit_http_server(http_server *server);

/*
 * Allocates on the heap and initializes a new server.
 * Internally calls init_http_server, for more info check there.
 * NOTE: memory must be free'd and deinitialize via a call to 
 * free_http_server
 */
static inline http_server *new_http_server(
    char *port,
    int thr_count,
    int backlog
    ) {
  return init_http_server(
      malloc(sizeof(http_server)),
      port, 
      thr_count,
      backlog
      );
}

/*
 * Deinitializes and frees the memory owned by server.
 * NOTE: safe to call with NULL ptr
 */
static inline void free_http_server(http_server *server) {
  if (server) {
    deinit_http_server(server);
    free(server);
  }
}

#endif // HTTP_SERVER_H

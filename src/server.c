#define _POSIX_C_SOURCE 200112L 

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"
#include "../utils/logger.h"

http_server *init_http_server(
    http_server *server,
    char *port,
    int thr_count,
    int backlog
    ) {
  if (server == NULL)
    return NULL;

  if (strlen(port) >= 6) {
    log_error("Invalid number for port.");
    return NULL;
  }

  int status = 0;
  struct addrinfo hints = {0}, *p;
  memset(&hints, 0, sizeof(struct sockaddr));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, port, &hints, &server->serv_info);
  if (status != 0) {
    log_debug("Could not get address info");
    return NULL;
  }

  for (p = server->serv_info; p != NULL; p = p->ai_next) {
    server->socket_fd = socket(
        p->ai_family,
        p->ai_socktype,
        p->ai_protocol
        );
    if (server->socket_fd < 0)
      continue;

    status = bind(
        server->socket_fd,
        p->ai_addr,
        p->ai_addrlen
        );
    if (status < 0) {
      log_debug("Could not bind server to port: [%s]", port);
      goto cleanup;
    }
    
    status = listen(server->socket_fd, backlog);
    if (status < 0) {
      log_debug("Could not listen on socket, closing up server");
      goto cleanup;
    }

    /* fill in the server data */
    server->backlog = backlog;
    if (init_thpool(&server->thpool, thr_count) == NULL) {
      log_debug("Could not initialize thread pool");
      goto cleanup;
    }

    inet_pton(
        server->serv_info->ai_family,
        server->serv_info->ai_addr->sa_data,
        server->ip_str
        );
    memcpy(server->port, port, strlen(port));
    server->port[strlen(port)] = '\0';
    return server;
  }

cleanup:
  close(server->socket_fd);
  freeaddrinfo(server->serv_info);
  return NULL;
}

void deinit_http_server(http_server *server) {
  if (server) {
    deinit_thpool(&server->thpool);
    freeaddrinfo(server->serv_info);
    close(server->socket_fd);
    server->backlog = 0;
  }
}



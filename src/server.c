#define _POSIX_C_SOURCE 200112L 

#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"
#include "logger.h"

http_server *init_http_server(
    http_server *server,
    uint16_t port,
    uint32_t addr,
    int thr_count,
    int backlog
    ) {

  if (server == NULL)
    return NULL;

  if (init_connection(
        &server->connection,
        socket(AF_INET, SOCK_STREAM, 0)) == NULL) {
    log_debug("Could not initialize connection on new socket");
    return NULL;
  }

  con_set_port(&server->connection, htons(port));
  con_set_addr(&server->connection, htonl(addr));
  if (con_bind(&server->connection) != 0) {
    log_debug("Could not bind connection to port: [%d]", port);
    deinit_connection(&server->connection);
    return NULL;
  }

  if (con_listen(&server->connection, backlog) != 0) {
    log_debug("Could not listen on connection");
    deinit_connection(&server->connection);
    return NULL;
  }

  if (init_thpool(&server->thr_pool, thr_count) == NULL) {
    log_debug("Could not initialize thread pool");
    deinit_connection(&server->connection);
    return NULL;
  }

  return server;
}

void deinit_http_server(http_server *server) {
  if (server) {
    deinit_connection(&server->connection);
    deinit_thpool(&server->thr_pool);
  }
}

void handle_client(void *arg) {
  con *conn = (con*) arg;
  log_info("Handling client with fd: [%d]", conn->fd);
  string_cpy(
      &conn->outgoing,
      "HTTP/1.0 200 OK \r\n \r\nHello world\n"
      );

  write(conn->fd, conn->outgoing.buffer, conn->outgoing.len);
  log_info("Successfully responded to client");
  free_connection(conn);
}

void server_start(http_server *server) {
  thpool_run(&server->thr_pool);
  log_info("Server is now ready to accept client");
  while (1) {
    con *client = con_accept(&server->connection);
    log_info("Accepted client with fd: [%d]", client->fd);
    thpool_addwork(&server->thr_pool, handle_client, client);
  }
}



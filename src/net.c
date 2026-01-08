#define _POSIX_C_SOURCE 200112L 
#include "net.h"
#include <unistd.h>

con *init_connection(con *connection, int fd) {
  if (connection == NULL)
    return NULL;

  *connection = (con) {
    .fd = fd,
    .is_closing = 0,
    .is_draining = 0,
    .is_reading = 0,
    .is_responding = 0,
    .is_readable = 0,
    .is_writeable = 0
  };

  init_string(&connection->incoming, 128);
  init_string(&connection->outgoing, 128);
  return connection;
}

void deinit_connection(con *connection) {
  if (connection) {
    close(connection->fd);
    init_connection(connection, -1);
  }
}

int con_bind(con *connection) {
  if (connection)
    return bind(
        connection->fd, 
        (struct sockaddr*) &connection->loc.addr,
        sizeof(struct sockaddr_in)
        );
  else
   return -1;
}

int con_listen(con *connection, int backlog) {
  if (connection == NULL)
    return -1;

  return listen(connection->fd, backlog);
}

int con_close(con *connection) {
  if (connection)
    return close(connection->fd);
  else 
    return -1;
}

void con_set_port(con *connection, uint16_t port) {
  if (connection) {
    connection->loc.port = port;
    connection->loc.addr.sin_port = port;
  }
}

void con_set_addr(con *connection, uint32_t address) {
  if (connection)
    connection->loc.addr.sin_addr.s_addr = address;
}

void con_set_addr_structure(
    con *connection,
    struct sockaddr_in *addr
    ) {
  
  if (connection) 
    connection->loc.addr = (struct sockaddr_in) {
      .sin_addr = addr->sin_addr,
      .sin_port = addr->sin_port,
      .sin_family = addr->sin_family
    };
}

con *con_accept(con *connection) {
  int client_fd;
  struct sockaddr_in cli_addr;
  socklen_t cli_len = sizeof(cli_addr);

  client_fd = accept(
      connection->fd,
      (struct sockaddr *) &cli_addr,
      &cli_len
      );

  if (client_fd < 0)
    return NULL;

  con *client = new_connection(client_fd);
  con_set_port(client, connection->loc.port);
  con_set_addr_structure(client, &cli_addr);
  return client;
}

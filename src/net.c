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

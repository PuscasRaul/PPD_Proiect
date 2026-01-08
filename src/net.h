#ifndef NET_H
#define NET_H

#include "my_string.h"
#include <stdlib.h>
#include <netdb.h>
#include <stdint.h>

typedef struct con con;
typedef struct con_adr con_adr;
typedef void (*ev_handler)(con *);

struct con_adr {
  uint16_t port; 
  char ip_str[INET6_ADDRSTRLEN];
  struct sockaddr_in addr;
};

struct con {
  int fd;
  con_adr loc; /* Local address */
  unsigned is_reading : 1; /* Consuming received data */
  unsigned is_draining : 1; /* Send remaining data then close and free immediately */
  unsigned is_closing : 1; /* Close and free the connection immediately */
  unsigned is_responding :1; /* Response is still being generated */
  unsigned is_readable: 1; /* Connection is ready to read */
  unsigned is_writeable: 1; /* Connection is ready to write */

  string_t incoming; /* Buffer for reading request into */
  string_t outgoing; /* Buffer for writing responso to */

  time_t timer; /* Active timer */
};

con *init_connection(con *connection, int fd);
void deinit_connection(con *connection);

static inline con *new_connection(int fd) {
  return init_connection(malloc(sizeof(con)), fd);
}

static inline void free_connection(con *connection) {
  if (connection) {
    deinit_connection(connection);
    free(connection);
  }
}

void con_set_port(con *connection, uint16_t port);
void con_set_addr(con *connection, uint32_t address);
void con_set_addr_structure(con *connection, struct sockaddr_in *addr);

int con_bind(con *connection);
int con_listen(con *connection, int backlog);
int con_close(con *connection);
con *con_accept(con *connection);
#endif // NET_H

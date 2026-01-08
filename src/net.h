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
  unsigned is_listening: 1; /* Connection listening on port */
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

/* 
 * Initializes a new connection 
 * param: con: connection to be initialized
 * param: fd: open file descriptor for the connection
 * return: NULL on failure
 *         address of connection on success
 * NOTE: deinitializea via a call to deinit_connection
 */
con *init_connection(con *connection, int fd);

/*
 * Deinitializes a connection
 * Closes the fd and clears up all the memory kept inside of the con
 * con: connection to be deinitializes
 * NOTE: safe to call with NULL ptr
 */
void deinit_connection(con *connection);

/*
 * Allocates on the heap and initializes a new connection for a 
 * file descriptor
 * param: fd: file descriptor for the connection
 *
 * return: NULL on failure
 *         address of new connection on success
 */
static inline con *new_connection(int fd) {
  return init_connection(malloc(sizeof(con)), fd);
}

/*
 * Deinitializes and frees the connection
 * internally calls deinit_connection and then free
 * NOTE: safe to call with NULL ptr
 */
static inline void free_connection(con *connection) {
  if (connection) {
    deinit_connection(connection);
    free(connection);
  }
}

/*
 * Sets the port both at the surface level and inside the addr struct
 * param: connection: connection to set port for
 * param: port: network-byte order port
 */
void con_set_port(con *connection, uint16_t port);

/*
 * Set the address.
 * param: connection: connection to set address for
 * param: address: network-byte order address
 */
void con_set_addr(con *connection, uint32_t address);

/*
 * Set the address structure
 * param: connection: connection to set address for
 * param: address: struct sockaddr_in to deep copy
 */
void con_set_addr_structure(con *connection, struct sockaddr_in *addr);

/*
 * Binds connection to port
 * return: 0 on success
 *         != 0 on failure/ NULL connection
 */
int con_bind(con *connection);

/*
 * Listens on port
 * NOTE: must call con_bind before
 *
 * return: 0 on success
 *         != 0 on failure/ NULL connection
 */
int con_listen(con *connection, int backlog);

/*
 * Closes the connection
 * return: 0 on success
 *         != 0 on failure/ NULL connection
 */
int con_close(con *connection);

/*
 * Accepts and initializes a new connection to connection->fd
 * return: NULL on failure
 *         client connection on success
 */
con *con_accept(con *connection);
#endif // NET_H

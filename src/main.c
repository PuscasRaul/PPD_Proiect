#define _POSIX_C_SOURCE 200112L 

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils/logger.h"
#include "http/server.h"

#define PORT "34900"
#define BACKLOG 10

int main() {

  char *response = malloc(256);
  int conn_fd;
  socklen_t sin_size;
  struct sockaddr conn_addr;
  http_server server;
  if (init_http_server(&server, PORT, 16, BACKLOG) == NULL) {
    log_error("Could not initialize server");
    return 1;
  }
  log_info("Server started successfully");

  for (; ;) {
    conn_fd = accept(
        server.socket_fd,
        (struct sockaddr *) &conn_addr,
        &sin_size
        ); 
    if (conn_fd < 0)
      continue;

    log_info("Accepted connection with fd: [%d]", conn_fd);

    response = "HTTP/1.0 200 OK \r\n"
      "\r\n"
      "Hello world\n";
    write(conn_fd, response, 32);
    close(conn_fd);
    
  }

  deinit_http_server(&server);
  free(response);
  return 0;
}

#define _POSIX_C_SOURCE 200112L 

#include "src/logger.h"
#include "src/server.h"

#define PORT "34900"
#define BACKLOG 10

int main() {
  http_server server;
  if (init_http_server(&server, 34900, INADDR_ANY, 16, 32) == NULL) {
    log_error("Could not initialize server");
    return -1;
  }

  log_info(
      "Initialized server on port: [%d], addr: [%ld]", 
      server.connection.loc.port,
      INADDR_ANY
      );
  log_info("Now starting...");

  server_start(&server);
  return 0;
}

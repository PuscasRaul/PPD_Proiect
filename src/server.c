#define _GNU_SOURCE
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <limits.h>

#include "server.h"
#include "logger.h"
#include "request.h"

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
    http_request req;
    init_http_request(&req);

    // 1. READ HEADERS LOOP
    char *header_end = NULL;
    header_end = memmem(
        conn->incoming.buffer,
        conn->incoming.len,
        "\r\n\r\n",
        4);
    while (header_end == NULL) {
        size_t space_left = conn->incoming.capacity - conn->incoming.len;
        if (space_left < 128) { 
            string_resize(&conn->incoming, conn->incoming.capacity * 2);
            space_left = conn->incoming.capacity - conn->incoming.len;
        }

        ssize_t b = read(
            conn->fd,
            conn->incoming.buffer + conn->incoming.len,
            space_left
            );

        if (b <= 0) {
            log_error("Connection closed or read error while waiting for headers");
            goto cleanup;
        }
        conn->incoming.len += b;
        header_end = memmem(
            conn->incoming.buffer,
            conn->incoming.len,
            "\r\n\r\n",
            4);
    }

    size_t header_bytes = (header_end - conn->incoming.buffer) + 4;
    string_view cl_view;
    if (lookup_header_value(
          conn->incoming.buffer,
          header_bytes,
          "Content-Length",
          &cl_view) == 0) {
        
        long content_length = 0;
        for (size_t i = 0; i < cl_view.len; i++) {
            if (cl_view.buffer[i] >= '0' && cl_view.buffer[i] <= '9')
                content_length = content_length * 10 + 
                                 (cl_view.buffer[i] - '0');
        }

        size_t total_expected = header_bytes + content_length;
        if (conn->incoming.capacity < total_expected) 
            string_resize(&conn->incoming, total_expected + 1);

        while (conn->incoming.len < total_expected) {
            ssize_t b = read(conn->fd, 
                             conn->incoming.buffer + conn->incoming.len, 
                             total_expected - conn->incoming.len);
            if (b <= 0) break;
            conn->incoming.len += b;
        }
        init_str_view(
            &req.body,
            conn->incoming.buffer + header_bytes,
            content_length
            );
    }

    int line_offset = http_extract_req_line(
        conn->incoming.buffer,
        header_bytes,
        &req
        );

    if (line_offset > 0) {
        http_extract_headers(
            conn->incoming.buffer + line_offset + 2,
            header_bytes - (line_offset + 2),
            &req
            );
    }

    string_cpy(
        &conn->outgoing,
        "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world\n"
        );
    write(conn->fd, conn->outgoing.buffer, conn->outgoing.len);

cleanup:
    deinit_http_request(&req);
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

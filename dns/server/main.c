#include <stdio.h>

#include "io_utils.h"
#include "dns_server.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s PORT\n", argv[0]);
    return 2;
  }
  const char* arg_port = argv[1];
  uint16_t port;

  if (parse_u16(arg_port, &port) < 0) {
    fprintf(stderr, "Invalid port '%s'\n", arg_port);
    perror(NULL);
    return 2;
  }

  dns_server_t server;
  if (dns_server_init(&server, port) < 0) {
    perror("Error initializing dns server");
    return 1;
  }

  dns_server_serve(&server);

  // TODO: sigterm handling
  dns_server_destroy(&server);
}

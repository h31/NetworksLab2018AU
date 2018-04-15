#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "io_utils.h"
#include "dns_client.h"


int main(int argc, char** argv) {
  if (argc < 3 || argc > 4) {
    printf("Usage: %s DNS_SERVER NAME [PORT]\n", argv[0]);
    return 2;
  }
  const char* arg_server_addr = argv[1];
  const char* arg_name = argv[2];
  const char* arg_port = argc > 3 ? argv[3] : NULL;

  uint16_t port = DNS_DEFAULT_PORT;
  if (arg_port != NULL && parse_u16(arg_port, &port) < 0) {
    fprintf(stderr, "Invalid port '%s'\n", arg_port);
    perror(NULL);
    return 2;
  }

  dns_client_t dns_client;
  if (dns_client_init(&dns_client, arg_server_addr, port)) {
    perror("Error initializing dns client");
    return 1;
  }

  in_addr_t result;
  if (dns_client_request(&dns_client, arg_name, &result) < 0) {
    perror("Error making dns request");
    return 1;
  }

  printf("%s\n", inet_ntoa((struct in_addr) {result}));
  dns_client_destroy(&dns_client);
}

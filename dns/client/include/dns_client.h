#ifndef AUDNS_DNS_CLIENT_H
#define AUDNS_DNS_CLIENT_H

#include <stdint.h>

#include "dns_proto.h"


typedef struct dns_server {
  struct sockaddr_in server_addr;
  uint16_t next_id;
  int socket;
} dns_client_t;

int dns_client_init(dns_client_t* client, const char* server, uint16_t port);

void dns_client_destroy(dns_client_t* client);

int dns_client_request(dns_client_t* client, const char* name, in_addr_t* result);

#endif  // AUDNS_DNS_CLIENT_H

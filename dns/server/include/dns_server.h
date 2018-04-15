#ifndef AUDNS_DNS_SERVER_H
#define AUDNS_DNS_SERVER_H

#include <stdint.h>
#include "dns_proto.h"


typedef struct dns_server {
  int socket;
} dns_server_t;

int dns_server_init(dns_server_t* server, uint16_t port);

void dns_server_destroy(dns_server_t* server);

int dns_server_serve(dns_server_t* server);


#endif  // AUDNS_DNS_SERVER_H

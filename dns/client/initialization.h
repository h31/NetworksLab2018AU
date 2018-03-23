#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

int get_udp_sock();
struct sockaddr_in get_addr_struct(char *server_addr, uint16_t port);

#endif /* INITIALIZATION_H_ */

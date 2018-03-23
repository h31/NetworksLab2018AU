#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#define MAX_UDP_DATA_SIZE (65507)

int get_udp_sock();
struct sockaddr_in get_external_addr(char *server_addr, uint16_t port);
struct sockaddr_in bind_to_port(int sock, uint16_t port);

#endif /* CONNECTION_H_ */

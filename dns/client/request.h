#ifndef REQUEST_H_
#define REQUEST_H_

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t* request_addr(int sock, struct sockaddr_in *server_addr, char *name);

#endif /* REQUEST_H_ */

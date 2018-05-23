#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"

int get_udp_sock() {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == -1) {
		perror("socket() failed!");
		exit(1);
	}

	return sock;
}

struct sockaddr_in get_server_addr(char *server_addr, uint16_t port) {
	struct sockaddr_in addr;
	memset((char*) &addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (inet_aton(server_addr, &addr.sin_addr) == 0) {
		perror("inet_aton() failed!");
		exit(1);
	}

	return addr;
}

struct sockaddr_in bind_to_port(int sock, uint16_t port) {
	struct sockaddr_in addr;
	memset((char*) &addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if(bind(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		perror("bind() failed!");
		exit(1);
	}

	return addr;
}


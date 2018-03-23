#include "initialization.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_udp_sock() {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == -1) {
		perror("socket() failed!");
		exit(1);
	}

	return sock;
}

struct sockaddr_in get_addr_struct(char *server_addr, uint16_t port) {
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


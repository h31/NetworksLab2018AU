#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "initialization.h"
#include "request.h"
#include "response.h"

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage %s host port nickname\n", argv[0]);
		exit(0);
	}

    int sock = get_udp_sock();
    struct sockaddr_in addr = get_addr_struct(argv[1], (uint16_t) atoi(argv[2]));

    uint8_t *response = request_addr(sock, &addr, argv[3]);
    process_response(response);

    close(sock);

    return 0;
}


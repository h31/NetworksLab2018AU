#include <stdio.h>
#include <stdlib.h>

#include "../common/connection.h"
#include "../common/handling.h"

uint8_t data_buffer[MAX_UDP_DATA_SIZE];
char message_buffer[MAX_UDP_DATA_SIZE];

uint8_t generate_value(char *string, uint8_t salt) {
	int value = 1;
	int mod = 263;

	char *ptr = string;
	while (*ptr) {
		int character = *(ptr++);
		value = (value * (character + salt)) % mod;
	}

	return value % 256;
}

address_t generate_address(char *string) {
	address_t result;

	for (int i = 0; i < 4; ++i) {
		result.bytes[i] = generate_value(string, i);
	}

	return result;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage %s port\n", argv[0]);
		exit(0);
	}

    int sock = get_udp_sock();
    bind_to_port(sock, (uint16_t) atoi(argv[1]));

    while(1) {
    	struct sockaddr_in client_addr;

    	socklen_t addr_len = sizeof(client_addr);
    	if (recvfrom(sock, data_buffer, MAX_UDP_DATA_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len) == -1) {
            perror("recvfrom() failed");
            exit(1);
    	}

    	get_query_name(data_buffer, message_buffer);
    	address_t address = generate_address(message_buffer);

    	uint8_t *ptr = form_response(data_buffer, address);
        if (sendto(sock, data_buffer, ptr - data_buffer, 0, (struct sockaddr*) &client_addr, addr_len) == -1) {
            perror("sendto() failed!");
            exit(1);
        }
    }

    close(sock);

    return 0;
}

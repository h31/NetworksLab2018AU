#include "communication.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

const uint16_t max_string_len = UINT16_MAX;

void send_uint16(int sock, uint16_t data) {
	char buffer[2] = {data & 0xFF, (data >> 8) & 0xFF};

	if (write(sock, buffer, 2) < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}

void send_cstring(int sock, char *string) {
	size_t length = strlen(string);

	if (length > max_string_len) {
		perror("ERROR: attempt to send a long string");
		exit(1);
	}

	send_uint16(sock, length);

	if (write(sock, string, strlen(string)) < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}

uint16_t receive_uint16(int sock) {
	char buffer[2];

	if (read(sock, buffer, 2) < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	return buffer[0] | (buffer[1] << 8);
}

char* receive_cstring(int sock) {
	uint16_t length = receive_uint16(sock);

	char *string = (char*)malloc(length + 1);

	if (read(sock, string, length) < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	string[length] = 0;
	return string;
}

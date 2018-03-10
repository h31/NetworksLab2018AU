#include "communication.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

ssize_t receive_uint16(int sock, uint16_t *data) {
	char buffer[2];

	int ret_code = read(sock, buffer, 2);
	if (ret_code == 0) {
		return 0;
	}

	if (ret_code != 2) {
		perror("ERROR reading from socket");
		exit(1);
	}

	*data = buffer[0] | (buffer[1] << 8);
	return ret_code;
}

ssize_t receive_cstring(int sock, char *data) {
	uint16_t length;
	ssize_t ret_code1 = receive_uint16(sock, &length);
	if (ret_code1 == 0) {
		return 0;
	}

	ssize_t ret_code2 = read(sock, data, length);
	if (ret_code2 == 0) {
		return 0;
	}

	if (ret_code2 != length) {
		perror("ERROR reading from socket");
		exit(1);
	}

	data[length] = 0;
	return ret_code1 + ret_code2;
}

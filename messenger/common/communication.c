#include "communication.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ssize_t send_data(int sock, void *data, size_t size) {
	if (size > MAX_CHUNK_LEN) {
		perror("ERROR: attempt to send long data chunk");
		exit(1);
	}

	void *ptr = data;
	void *end = data + size;

	while (ptr != end) {
		ssize_t bytes_written = send(sock, ptr, end - ptr, 0);
		if (bytes_written < 0) {
			return bytes_written;
		}

		ptr += bytes_written;
	}

	return size;
}

ssize_t send_uint16(int sock, uint16_t data) {
	uint8_t buffer[2] = {data & 0xFF, (data >> 8) & 0xFF};
	return send_data(sock, buffer, 2);
}

ssize_t send_cstring(int sock, char *string) {
	size_t length = strlen(string);

	ssize_t bytes_written1 = send_uint16(sock, length);
	if (bytes_written1 < 0) {
		return bytes_written1;
	}

	ssize_t bytes_written2 = send_data(sock, string, length);
	if (bytes_written2 < 0) {
		return bytes_written2;
	}

	return bytes_written1 + bytes_written2;
}

ssize_t receive_data(int sock, void *data, size_t size) {
	void *ptr = data;
	void *end = data + size;

	while (ptr != end) {
		ssize_t bytes_read = recv(sock, ptr, end - ptr, 0);
		if (bytes_read <= 0) {
			return bytes_read;
		}

		ptr += bytes_read;
	}

	return size;
}

ssize_t receive_uint16(int sock, uint16_t *data) {
	uint8_t buffer[2];
	ssize_t bytes_read = receive_data(sock, buffer, 2);

	if (bytes_read <= 0) {
		return bytes_read;
	}

	*data = buffer[0] | (buffer[1] << 8);
	return bytes_read;
}

ssize_t receive_cstring(int sock, char *string) {
	uint16_t length;
	ssize_t bytes_read1 = receive_uint16(sock, &length);
	if (bytes_read1 <= 0) {
		return bytes_read1;
	}

	ssize_t bytes_read2 = receive_data(sock, string, length);
	if (bytes_read2 <= 0) {
		return bytes_read2;
	}

	string[length] = 0;
	return bytes_read1 + bytes_read2;
}

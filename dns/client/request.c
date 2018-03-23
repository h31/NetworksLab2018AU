#include "request.h"
#include "../common/connection.h"

void convert_name_to_byte_sequence(char *name, uint8_t *dest) {
	size_t len = 0;
	uint8_t *len_ptr = dest;
	uint8_t *cpy_ptr = dest + 1;

	char *name_ptr = name;

	while (1) {
		char current_char = *(name_ptr++);

		if (current_char == '.' || current_char == '\0') {
			*len_ptr = len;
			len = 0;
			len_ptr = cpy_ptr++;

			if (current_char == '\0') {
				break;
			}
		} else {
			*(cpy_ptr++) = current_char;
			++len;
		}
	}

	*len_ptr = 0;
}

uint8_t* write_request_header(uint8_t *dest) {
	*(dest++) = 0x00;
	*(dest++) = 0x00; // ID!!!

	*(dest++) = 0x01;
	*(dest++) = 0x00;

	*(dest++) = 0x00;
	*(dest++) = 0x01;

	*(dest++) = 0x00;
	*(dest++) = 0x00;

	*(dest++) = 0x00;
	*(dest++) = 0x00;

	*(dest++) = 0x00;
	*(dest++) = 0x00;

	return dest;
}

uint8_t* write_request_footer(uint8_t *dest) {
	*(dest++) = 0x00;
	*(dest++) = 0x01;

	*(dest++) = 0x00;
	*(dest++) = 0x01;

	return dest;
}

uint8_t buffer[MAX_UDP_DATA_SIZE];

uint8_t* request_addr(int sock, struct sockaddr_in *server_addr, char *name) {
	size_t sequence_size = strlen(name) + 2;

	uint8_t *name_as_bytes = malloc(sequence_size);
	convert_name_to_byte_sequence(name, name_as_bytes);

	uint8_t *buff_ptr = write_request_header(buffer);

	memcpy(buff_ptr, name_as_bytes, sequence_size);
	buff_ptr += sequence_size;

	buff_ptr = write_request_footer(buff_ptr);

	if (sendto(sock, buffer, buff_ptr - buffer, 0, (struct sockaddr*) server_addr, sizeof(*server_addr)) == -1) {
		perror("sendto() failed!");
		exit(1);
	}

	memset(buffer, 0, MAX_UDP_DATA_SIZE);
	socklen_t addr_len = sizeof(*server_addr);
	if (recvfrom(sock, buffer, MAX_UDP_DATA_SIZE, 0, (struct sockaddr*) server_addr, &addr_len) == -1) {
		perror("recvfrom() failed!");
		exit(1);
	}

	free(name_as_bytes);

	return buffer;
}

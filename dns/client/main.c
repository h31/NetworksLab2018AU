#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#define MAX_UDP_DATA_SIZE (65507)
uint8_t buffer[MAX_UDP_DATA_SIZE];

int get_sock() {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == -1) {
		perror("socket() failed!");
		exit(1);
	}

	return sock;
}

struct sockaddr_in get_addr(char *server_addr, uint16_t port) {
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

uint16_t get_num_of_questions(uint8_t *response) {
	return response[5] | (response[4] << 8);
}

uint16_t get_num_of_answers(uint8_t *response) {
	return response[7] | (response[6] << 8);
}

uint8_t* skip_response_header(uint8_t *response) {
	return response + 12;
}

uint8_t* skip_response_query(uint8_t *response) {
	while (*response) {
		response += *response + 1;
	}

	return response + 5;
}

uint8_t* process_response_answer(uint8_t *response) {
	response += 12;

	printf("%d.%d.%d.%d\n", response[0], response[1], response[2], response[3]);

	return response + 4;
}

void process_response(uint8_t *response) {
	uint16_t num_of_questions = get_num_of_questions(response);
	uint16_t num_of_answers = get_num_of_answers(response);

	response = skip_response_header(response);
	while (num_of_questions-- > 0) {
		response = skip_response_query(response);
	}

	while (num_of_answers-- > 0) {
		response = process_response_answer(response);
	}
}

int main(void) {
    int sock = get_sock();
    struct sockaddr_in addr = get_addr("8.8.8.8", 53);

    uint8_t *response = request_addr(sock, &addr, "google.com");
    process_response(response);

    close(sock);

    return 0;
}


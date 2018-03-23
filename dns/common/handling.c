#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handling.h"

uint16_t get_num_of_questions(uint8_t *response) {
	return response[5] | (response[4] << 8);
}

uint16_t get_num_of_answers(uint8_t *response) {
	return response[7] | (response[6] << 8);
}

uint8_t* skip_header(uint8_t *response) {
	return response + 12;
}

uint8_t* skip_question(uint8_t *response) {
	while (*response) {
		response += *response + 1;
	}

	return response + 5;
}

uint8_t* print_response_answer(uint8_t *response) {
	response += 12;

	printf("%d.%d.%d.%d\n", response[0], response[1], response[2], response[3]);

	return response + 4;
}

void print_server_response(uint8_t *response) {
	uint16_t num_of_questions = get_num_of_questions(response);
	uint16_t num_of_answers = get_num_of_answers(response);

	response = skip_header(response);
	while (num_of_questions-- > 0) {
		response = skip_question(response);
	}

	while (num_of_answers-- > 0) {
		response = print_response_answer(response);
	}
}

void get_query_name(uint8_t *query, char *dest) {
	query = skip_header(query);

	while (*query) {
		uint8_t *ptr = query + 1;
		while (ptr != query + *query + 1) {
			*(dest++) = *(ptr++);
		}

		*(dest++) = '.';

		query = ptr;
	}

	*(dest - 1) = '\0';
}

#ifndef HANDLING_H_
#define HANDLING_H_

#include <stdint.h>

typedef struct address {
	uint8_t bytes[4];
} address_t;

void print_server_response(uint8_t *response);
void get_query_name(uint8_t *query, char *dest);
uint8_t* form_response(uint8_t *query, address_t address);

#endif /* HANDLING_H_ */

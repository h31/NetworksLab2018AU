#ifndef HANDLING_H_
#define HANDLING_H_

#include <stdint.h>

void print_server_response(uint8_t *response);
void get_query_name(uint8_t *query, char *dest);

#endif /* HANDLING_H_ */

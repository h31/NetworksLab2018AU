#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdint.h>

const uint16_t max_string_len;

void send_uint16(int sock, uint16_t data);
void send_cstring(int sock, char *string);

uint16_t receive_uint16(int sock);
char* receive_cstring(int sock);

#endif /* COMMUNICATION_H_ */

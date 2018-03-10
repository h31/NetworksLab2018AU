#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdint.h>

#include <unistd.h>

#define MAX_STRING_LEN (1000)

void send_uint16(int sock, uint16_t data);
void send_cstring(int sock, char *string);

ssize_t receive_uint16(int sock, uint16_t *data);
ssize_t receive_cstring(int sock, char *data);

#endif /* COMMUNICATION_H_ */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdint.h>

#include <unistd.h>

#define MAX_MSG_LEN (1000)

ssize_t send_data(int sock, void *data, size_t size);
ssize_t send_uint16(int sock, uint16_t data);
ssize_t send_cstring(int sock, char *string);

ssize_t receive_data(int sock, void *data, size_t size);
ssize_t receive_uint16(int sock, uint16_t *data);
ssize_t receive_cstring(int sock, char *string);

#endif /* COMMUNICATION_H_ */

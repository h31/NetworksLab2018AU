#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <memory.h>
#include "vector.h"
#include <WinSock2.h>

static int read_message(SOCKET sock, vector_t* msg) {
    uint32_t size = 0;
	int n = recv(sock, (char*)&size, 4, 0);
    if (n <= 0) {
        perror("ERROR: read from socket");
        return -1;
    }
    char* message = malloc(sizeof(char) * (size + 1));
    uint32_t read_size = 0;
    while (read_size < size) {
        n = recv(sock, message + read_size, size - read_size, 0);
        if (n <= 0) {
            perror("ERROR: read from socket");
            return -1;
        }
        read_size += n;
    }
    message[size] = '\0';
    msg->size = size;
    msg->capacity = size + 1;
    msg->data = message;
    return 0;
}

static int write_message(SOCKET sock, vector_t* msg) {
    int n = send(sock, (const char*) &msg->size, 4, 0);
    if (n < 0) {
        perror("ERROR: write size to socket");
        return -1;
    }
    n = send(sock, msg->data, msg->size, 0);
    if (n < 0) {
        perror("ERROR: write message to socket");
        return -1;
    }
    return 0;
}

#endif //MESSAGE_H

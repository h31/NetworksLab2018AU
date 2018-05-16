#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "vector.h"

static int read_message(int sockfd, vector_t* msg) {
    uint32_t size = 0;
    ssize_t n = read(sockfd, &size, 4);
    if (n <= 0) {
        perror("ERROR: read from socket");
        return -1;
    }
    char* message = malloc(sizeof(char) * (size + 1));
    uint32_t read_size = 0;
    while (read_size < size) {
        n = read(sockfd, message + read_size, size - read_size);
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

static ssize_t write_message(int sockfd, vector_t* msg) {
    ssize_t n = write(sockfd, &msg->size, 4);
    if (n < 0) {
        perror("ERROR: write size to socket");
        return -1;
    }
    n = write(sockfd, msg->data, msg->size);
    if (n < 0) {
        perror("ERROR: write message to socket");
        return -1;
    }
    return 0;
}

#endif //MESSAGE_H

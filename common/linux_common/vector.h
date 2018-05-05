#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char* data;
    uint32_t size;
    uint32_t capacity;
} vector_t;

static void append(vector_t* msg, char elem) {
    if (msg->size < msg->capacity) {
        msg->data[msg->size++] = elem;
    } else {
        char* temp = malloc(sizeof(char) * (msg->size) * 2);
        bzero(temp, msg->size * 2);
        strncpy(temp, msg->data, msg->size);
        temp[msg->size] = elem;
        free(msg->data);
        msg->data = temp;
        msg->capacity = msg->size * 2;
        ++msg->size;
    }
}

static void append_line(vector_t* msg, char* buffer, size_t len) {
    if (msg->size + len <= msg->capacity) {
        strncpy(msg->data + msg->size, buffer, len);
        msg->size += len;
    } else {
        char* temp = malloc(sizeof(char) * (msg->size * 2 + len));
        bzero(temp, msg->size * 2 + len);
        strncpy(temp, msg->data, msg->size);
        strncpy(temp + msg->size, buffer, len);
        free(msg->data);
        msg->data = temp;
        msg->capacity = msg->size * 2 + len;
        msg->size += len;
    }
}

#endif // VECTOR_H

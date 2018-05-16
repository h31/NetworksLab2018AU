#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <string.h>
#include <memory.h>

typedef struct {
    char* data;
    uint32_t size;
    uint32_t capacity;
} vector_t;

static void append(vector_t* msg, char elem) {
	if (msg->size < msg->capacity) {
		msg->data[msg->size++] = elem;
	} else {
		msg->data = realloc(msg->data, sizeof(char) * (msg->size) * 2);
		msg->data[msg->size] = elem;
		msg->capacity = msg->size * 2;
		++msg->size;
	}
}

static void append_line(vector_t* msg, char* buffer, size_t len) {
	if (msg->size + len <= msg->capacity) {
		strncpy(msg->data + msg->size, buffer, len);
		msg->size += len;
	} else {
		msg->data = realloc(msg->data, sizeof(char) * (msg->size * 2 + len));
		strncpy(msg->data + msg->size, buffer, len);
		msg->capacity = msg->size * 2 + len;
		msg->size += len;
	}
}

static inline void free_vector(void* arg) {
	vector_t* temp = (vector_t*)arg;
	if (temp->data != NULL) {
		free(temp->data);
	}
}

#endif // VECTOR_H

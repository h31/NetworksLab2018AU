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
        char* temp = malloc(sizeof(char) * (msg->size) * 2);
		memset(temp, '\0', msg->size * 2);
		strncpy_s(temp, msg->size * 2, msg->data, msg->size);
		temp[msg->size] = elem;
        free(msg->data);
        msg->data = temp;
        msg->capacity = msg->size * 2;
        ++msg->size;
    }
}

static inline void free_vector(void* arg) {
	vector_t* temp = (vector_t*)arg;
	if (temp->data != NULL) {
		free(temp->data);
	}
}

#endif // VECTOR_H

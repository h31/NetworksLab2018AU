#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "vector.h"

typedef struct {
    vector_t name;
    int sockfd;
    pthread_t reader;
    pthread_t writer;
    pthread_mutex_t mutex;
    pthread_cond_t can_consume;
} client_t;

#endif //CLIENT_H

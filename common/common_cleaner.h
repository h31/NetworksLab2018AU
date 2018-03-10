#ifndef COMMON_CLEANER_H
#define COMMON_CLEANER_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "vector.h"
#include "client.h"
#include "message.h"

static void mutex_unlock(void* arg) {
    pthread_mutex_unlock((pthread_mutex_t*) arg);
}

static void thread_cancel(void* arg) {
    pthread_t* thread = (pthread_t*) arg;
    pthread_cancel(*thread);
}

static void close_socket(void* arg) {
    int sockfd = *((int*) arg);
    close(sockfd);
}

static inline void free_vector(void* arg) {
    free(((vector_t*)arg)->data);
}

static void free_client(client_t* client) {
    free_vector(&client->name);
}


#endif //COMMON_CLEANER_H

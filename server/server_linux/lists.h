#ifndef SERVER_LINUX_CONTAINERS_H
#define SERVER_LINUX_CONTAINERS_H

#include "vector.h"
#include "client.h"

struct msg_list {
    vector_t client_name;
    vector_t msg;
    struct msg_list* next;
};

struct thread_list {
    client_t client;
    struct thread_list* next;
};

typedef struct {
    struct thread_list* thread_head;
    struct msg_list* msg_head;
} pair_t;

#endif //SERVER_LINUX_CONTAINERS_H

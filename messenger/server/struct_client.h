#ifndef ELEGRAM_STRUCT_CLIENT_H
#define ELEGRAM_STRUCT_CLIENT_H

#include "server.h"

struct client {
  const int sock_fd;  // GUARDED_BY(mutex)
  pthread_mutex_t mutex;
  struct server* server;
  struct list_head lnode;
  pthread_t receiver_thread;
};

void client_init(struct client* client, struct server* server, int sock_fd);

void destroy_client(struct client* client);

void* client_routine(void* arg_raw);

#endif  // ELEGRAM_STRUCT_CLIENT_H

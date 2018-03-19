#ifndef ELEGRAM_STRUCT_CLIENT_H
#define ELEGRAM_STRUCT_CLIENT_H

#include "socket_utils.h"
#include "server.h"

struct client {
  socket_t socket;  // GUARDED_BY(mutex)
  pthread_mutex_t mutex;
  struct server* server;
  struct list_head lnode;
  pthread_t receiver_thread;
};

void client_init(struct client* client, struct server* server, socket_t socket);

void destroy_client(struct client* client);

void* client_routine(void* arg_raw);

#endif  // ELEGRAM_STRUCT_CLIENT_H

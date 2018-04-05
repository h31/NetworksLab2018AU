#ifndef ELEGRAM_STRUCT_CLIENT_H
#define ELEGRAM_STRUCT_CLIENT_H

#include <stdatomic.h>
#include <stdbool.h>

#include "socket_utils.h"
#include "server.h"

struct client {
  socket_t socket;  // GUARDED_BY(socket_mutex)
  pthread_mutex_t socket_mutex;
  struct server* server;
  struct list_head lnode;
  pthread_t receiver_thread;
  atomic_bool finished;
};

void client_init(struct client* client, struct server* server, socket_t socket);

void destroy_client(struct client* client);

void* client_routine(void* arg_raw);

static inline void client_start(struct client* client) {
  pthread_create(&client->receiver_thread, NULL, client_routine, client);
}

static inline void client_stop(struct client* client) {
  pthread_cancel(client->receiver_thread);
}

static inline void client_join(struct client* client) {
  pthread_join(client->receiver_thread, NULL);
}

static inline bool client_finished(struct client* client) {
  return atomic_load(&client->finished);
}

#endif  // ELEGRAM_STRUCT_CLIENT_H

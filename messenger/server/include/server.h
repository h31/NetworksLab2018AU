#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "list.h"
#include "message_format.h"
#include "socket_utils.h"

struct server {
  struct list_head clients_list;  // GUARDED_BY(rwlock)
  const socket_t socket;
  pthread_rwlock_t rwlock;
};

void destroy_server(struct server* server);

int server_init(struct server* server, uint16_t port);

int server_broadcast_message(struct server* server, elegram_msg_t* message);

int server_serve(struct server* server);

#endif  // PROJECT_SERVER_H

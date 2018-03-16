#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "list.h"
#include "message_format.h"

struct server {
  struct list_head clients_list;  // GUARDED_BY(mutex)
  const int sock_fd;
  pthread_rwlock_t rwlock;
};

int server_init(struct server* server, uint16_t port);

int server_broadcast_message(struct server* server, elegram_msg_header header,
                             void* data, size_t data_length);

int server_serve(struct server* server);

#endif  // PROJECT_SERVER_H

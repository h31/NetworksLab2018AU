#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <message_format.h>

#include "thread_utils.h"
#include "message_format.h"
#include "list.h"
#include "struct_client.h"

void client_init(struct client* client, struct server* server, int sock_fd) {
  *client = (struct client) {
      .sock_fd = sock_fd,
      .server = server,
  };
  pthread_rwlock_wrlock(&server->rwlock);
  pthread_cleanup_push(cleanup_rwlock_unlock, &server->rwlock) ;
  list_push_back(&server->clients_list, &client->lnode);
  pthread_cleanup_pop(true);
}

void destroy_client(struct client* client) {
  pthread_rwlock_wrlock(&client->server->rwlock);
  close(client->sock_fd);
  pthread_mutex_destroy(&client->mutex);
  list_del(&client->lnode);
  pthread_rwlock_unlock(&client->server->rwlock);
}

static void cleanup_destroy_client(void* arg_raw) {
  destroy_client((struct client*) arg_raw);
}

void* client_routine(void* arg_raw) {
  struct client* client = (struct client*) arg_raw;

  pthread_cleanup_push(cleanup_destroy_client, client);
      while (true) {
        pthread_testcancel();

        elegram_msg_t message;
        if (read_message(&message, client->sock_fd) < 0) {
          perror("ERROR reading message");
          break;
        }

        pthread_cleanup_push(free, message.data) ;
            server_broadcast_message(client->server, &message);
        pthread_cleanup_pop(true);
      }
  pthread_cleanup_pop(true);

  return NULL;
};

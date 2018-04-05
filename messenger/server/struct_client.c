#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <message_format.h>

#include "thread_utils.h"
#include "list.h"
#include "struct_client.h"

void client_init(struct client* client, struct server* server, socket_t socket) {
  *client = (struct client) {
      .socket_mutex = PTHREAD_MUTEX_INITIALIZER,
      .socket = socket,
      .server = server,
  };
  atomic_init(&client->finished, false);
}

void destroy_client(struct client* client) {
  close_socket(client->socket);
  pthread_mutex_destroy(&client->socket_mutex);
}

static void cleanup_set_finished(void* client_raw) {
  struct client* client = (struct client*) client_raw;
  atomic_store(&client->finished, true);
}

void* client_routine(void* arg_raw) {
  struct client* client = (struct client*) arg_raw;

  pthread_cleanup_push(cleanup_set_finished, client) ;
      while (true) {
        pthread_testcancel();

        elegram_msg_t message;
        if (read_message(&message, client->socket) < 0) {
          print_error("ERROR reading message");
          break;
        }

        pthread_cleanup_push(free, message.data) ;
            server_broadcast_message(client->server, &message);
        pthread_cleanup_pop(true);
      }
  pthread_cleanup_pop(true);

  return NULL;
};

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <message_format.h>

#include "server.h"
#include "struct_client.h"
#include "thread_utils.h"


int server_init(struct server* server, uint16_t port) {
  *server = (struct server) {
      .clients_list = LIST_HEAD_INITIALIZER(server->clients_list),
      .socket = create_tcp_socket(),
      .rwlock = PTHREAD_RWLOCK_INITIALIZER,
  };

  if (server->socket < 0) {
    return -1;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = INADDR_ANY,
      .sin_port = htons(port),
  };

  /* Now bind the host address using bind() call.*/
  if (bind(server->socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    return -1;
  }

  return 0;
}

void destroy_server(struct server* server) {
  struct list_head* pos;
  struct list_head* tmp;
  list_for_each_safe(pos, tmp, &server->clients_list) {
    struct client* client = list_entry(pos, struct client, lnode);
    // `destroy_client` will be called by the client thread cleanup
    pthread_cancel(client->receiver_thread);
    pthread_join(client->receiver_thread, NULL);
    free(client);
  }
  close_socket(server->socket);
  pthread_rwlock_destroy(&server->rwlock);
}

int server_broadcast_message(struct server* server, elegram_msg_t* message) {
  struct list_head* pos;
  pthread_rwlock_rdlock(&server->rwlock);
  pthread_cleanup_push(cleanup_rwlock_unlock, &server->rwlock) ;
      time_t timestamp = time(NULL);
      message->header.timestamp = *gmtime(&timestamp);

      list_for_each(pos, &server->clients_list) {
        struct client* client = list_entry(pos, struct client, lnode);
        pthread_mutex_lock(&client->mutex);
        pthread_cleanup_push(cleanup_mutex_unlock, &client->mutex) ;
            write_message(message, client->socket);
        pthread_cleanup_pop(true);
      }
  pthread_cleanup_pop(true);
  return 0;
}

int server_serve(struct server* server) {
  listen(server->socket, 5);

  while (true) {
    pthread_testcancel();

    socket_t client_socket = accept(server->socket, NULL, NULL);
    if (client_socket < 0) {
      print_error("ERROR on accept");
      continue;
    }

    struct client* new_client = malloc(sizeof(struct client));
    client_init(new_client, server, client_socket);

    pthread_create(&new_client->receiver_thread, NULL, client_routine, new_client);
  }
}

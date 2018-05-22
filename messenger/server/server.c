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

int server_broadcast_message(struct server* server, elegram_msg_t* message) {
  struct list_head* pos;
  pthread_rwlock_rdlock(&server->rwlock);
  pthread_cleanup_push(cleanup_rwlock_unlock, &server->rwlock) ;
      time_t timestamp = time(NULL);
      message->header.timestamp = *gmtime(&timestamp);

      list_for_each(pos, &server->clients_list) {
        struct client* client = list_entry(pos, struct client, lnode);
        if (!client_finished(client)) {
          pthread_mutex_lock(&client->socket_mutex);
          pthread_cleanup_push(cleanup_mutex_unlock, &client->socket_mutex) ;
              // error code is deliberately ignored
              write_message(message, client->socket);
          pthread_cleanup_pop(true);
        }
      }
  pthread_cleanup_pop(true);
  return 0;
}

static void join_and_remove_client(struct client* client) {
  int old_cancel_state;
  // Even if a cancelation in this function might look safe, it's better to disable it
  // to avoid getting the system into an invalid state.
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_cancel_state);

  client_join(client);
  list_del(&client->lnode);
  client_destroy(client);
  free(client);

  pthread_setcancelstate(old_cancel_state, NULL);
}

void server_destroy(struct server* server) {
  int old_cancel_state;
  // cancelation in this destructor would cause an invalid state
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_cancel_state);

  struct list_head* pos;
  struct list_head* tmp;

  pthread_rwlock_wrlock(&server->rwlock);
  list_for_each_safe(pos, tmp, &server->clients_list) {
    struct client* client = list_entry(pos, struct client, lnode);
    client_stop(client);
    join_and_remove_client(client);
  }
  pthread_rwlock_unlock(&server->rwlock);

  close_socket(server->socket);
  pthread_rwlock_destroy(&server->rwlock);

  pthread_setcancelstate(old_cancel_state, NULL);
}

static void do_garbage_collection(struct server* server) {
  struct list_head* pos;
  struct list_head* tmp;

  pthread_rwlock_wrlock(&server->rwlock);
  pthread_cleanup_push(cleanup_rwlock_unlock, &server->rwlock);
      list_for_each_safe(pos, tmp, &server->clients_list) {
        pthread_testcancel();
        struct client* client = list_entry(pos, struct client, lnode);
        if (client_finished(client)) {
          join_and_remove_client(client);
        }
      }
  pthread_cleanup_pop(true);
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

    do_garbage_collection(server);

    struct client* new_client = malloc(sizeof(struct client));
    client_init(new_client, server, client_socket);

    pthread_rwlock_wrlock(&server->rwlock);
    list_push_back(&server->clients_list, &new_client->lnode);
    pthread_rwlock_unlock(&server->rwlock);

    client_start(new_client);
  }
}

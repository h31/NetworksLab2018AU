#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>

#include "server.h"
#include "struct_client.h"
#include "thread_utils.h"
#include "log.h"


static bool checked_read(int fd, void* buf, size_t count) {
  ssize_t res = read(fd, buf, count);

  if (res < 0) {
    LOG("Internal error: read failed");
    perror("read");
    return false;
  }
  if (res != count) {
    LOG("Internal error: read the data partially");
    return false;
  }
  return true;
}


int server_init(struct server* server, uint16_t port) {
  *server = (struct server) {
      .clients_list = LIST_HEAD_INITIALIZER(server->clients_list),
      .sock_fd = socket(AF_INET, SOCK_STREAM, 0),
  };

  if (server->sock_fd < 0) {
    return -1;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = INADDR_ANY,
      .sin_port = htons(port),
  };

  /* Now bind the host address using bind() call.*/
  if (bind(server->sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    return -1;
  }

  return 0;
}

int server_broadcast_message(struct server* server, elegram_msg_header header,
                                     void* data, size_t data_length) {
  struct list_head* pos;
  list_for_each(pos, &server->clients_list) {
    struct client* client = list_entry(pos, struct client, lnode);
    pthread_mutex_lock(&client->mutex);
    pthread_cleanup_push(cleanup_mutex_unlock, &client->mutex);
    write(client->sock_fd, &header, sizeof(header));
    write(client->sock_fd, data, data_length);
    pthread_cleanup_pop(true);
  }
  return 0;
}

int server_serve(struct server* server) {
  listen(server->sock_fd, 5);

  while (true) {
    pthread_testcancel();

    int client_sock_fd = accept(server->sock_fd, NULL, NULL);
    if (client_sock_fd < 0) {
      perror("ERROR on accept");
      continue;
    }

    pthread_t client_thread;
    struct client* new_client = malloc(sizeof(struct client));
    client_init(new_client, server, client_sock_fd);

    pthread_create(&client_thread, NULL, client_routine, new_client);
  }

  return 0;
}


void client_init(struct client* client, struct server* server, int sock_fd) {
  *client = (struct client) {
      .sock_fd = sock_fd,
      .mutex = PTHREAD_MUTEX_INITIALIZER,
      .server = server,
  };
  list_push_back(&server->clients_list, &client->lnode);
}

void* client_routine(void* arg_raw) {
  struct client* client = (struct client*) arg_raw;

  while (true) {
    pthread_testcancel();

    elegram_msg_header header_v1;
    if (!checked_read(client->sock_fd, &header_v1, sizeof(header_v1))) {
      return NULL;
    }
    if (elegram_header_checksum(header_v1) != header_v1.header_checksum) {
      LOG("Invalid header checksum: %u", header_v1.header_checksum);
      return NULL;
    }
    if (header_v1.message_offset < sizeof(header_v1)) {
      LOG("Invalid message format: .message_offset is too small: %u", header_v1.message_offset);
      return NULL;
    }
    if (header_v1.message_offset + header_v1.message_len > MAX_MESSAGE_LENGTH) {
      LOG("Too large message: %u bytes", header_v1.message_offset + header_v1.message_len);
      return NULL;
    }

    size_t data_length = header_v1.message_offset + header_v1.message_len - sizeof(header_v1);
    char* data = malloc(data_length);

    pthread_cleanup_push(free, data);
    read(client->sock_fd, data, data_length);
    server_broadcast_message(client->server, header_v1, data, data_length);
    pthread_cleanup_pop(true);
  }
};

#include <pthread.h>
#include <stdbool.h>

#include "server.h"
#include "stdio.h"
#include "log.h"


typedef struct {
  uint16_t port;
  /*out*/ int ret;
} server_routine_arg_t;

static void cleanup_destroy_server(void* server) {
  destroy_server((struct server*) server);
}

static void* server_routine(void* arg_raw) {
  server_routine_arg_t* arg = (server_routine_arg_t*) arg_raw;

  struct server server;
  LOG("[Server] initializing...");
  if (server_init(&server, arg->port) != 0) {
    perror("Error initializing server");
    arg->ret = -1;
    return NULL;
  }
  LOG("[Server] initialized");

  pthread_cleanup_push(cleanup_destroy_server, &server);
    LOG("[Server] working...");
    if (server_serve(&server) != 0)  {
      perror("Server error");
      arg->ret = -1;
    } else {
      arg->ret = 0;
    }
    LOG("[Server] finished");
  pthread_cleanup_pop(true);

  return NULL;
}

// Use EOF (C-d) to stop the server
int main(int argc, char* argv[]) {
  pthread_t server_thread;
  server_routine_arg_t arg = {
      .port = 5001,
  };
  pthread_create(&server_thread, NULL, server_routine, &arg);
  while (getchar() != EOF) {}
  pthread_cancel(server_thread);
  pthread_join(server_thread, NULL);

  if (arg.ret != 0) {
    perror("Server error");
    return 1;
  } else {
    return 0;
  }
}

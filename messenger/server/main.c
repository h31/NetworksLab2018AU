#include <pthread.h>

#include "server.h"
#include "stdio.h"
#include "log.h"


typedef struct {
  uint16_t port;
  /*out*/ int ret;
} server_routine_arg_t;

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

  LOG("[Server] working...");
  if (server_serve(&server) != 0)  {
    perror("Server error");
    arg->ret = -1;
    return NULL;
  }
  LOG("[Server] finished");

  arg->ret = 0;
  return NULL;
}

int main(int argc, char* argv[]) {
  pthread_t server_thread;
  server_routine_arg_t arg = {
    .port = 5001,
  };
  pthread_create(&server_thread, NULL, server_routine, &arg);
  pthread_join(server_thread, NULL);
  return arg.ret;
}

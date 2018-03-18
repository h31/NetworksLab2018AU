#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <zconf.h>
#include <message_format.h>

#include "thread_utils.h"
#include "elegram_port.h"
#include "message_format.h"


static void printf_now(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  fflush(stdout);
  va_end(args);
}

static int cli_get_message(elegram_msg_t* out, char* nickname) {
  printf_now("Enter message: ");

  char* line = NULL;
  size_t buf_len = 0;
  ssize_t text_length = getline(&line, &buf_len, stdin);
  if (text_length < 0) {
    return -1;
  }
  text_length += 1; // null character should be counted

  if (text_length > MAX_MESSAGE_LENGTH) {
    return EOVERFLOW;
  }

  *out = (elegram_msg_t) {
      .header = (elegram_msg_header_t) {
          .format_version = ELEGRAM_FORMAT_VERSION,
          .text_offset = 0,
          .text_length = (uint32_t) text_length,
          .header_checksum = elegram_header_checksum(out->header),
      },
      .data = line,
  };
  strcpy(out->header.nickname, nickname);

  return 0;
}

typedef struct {
  pthread_mutex_t screen_mutex;
  char nickname[32];
  int sock_fd;
} client_t;

static void send_message(client_t* client, const elegram_msg_t* message) {
  write_message(message, client->sock_fd);
}

static void* receiver_routine(void* arg) {
  client_t* client = (client_t*) arg;

  while (true) {
    pthread_testcancel();

    elegram_msg_t message;
    if (read_message(&message, client->sock_fd) < 0) {
      perror("ERROR reading message");
      return NULL;
    }

    char str_time[256];
    strftime(str_time, sizeof(str_time), "%x %H:%M", &message.header.timestamp);

    pthread_cleanup_push(free, message.data) ;
        pthread_mutex_lock(&client->screen_mutex);
        pthread_cleanup_push(cleanup_mutex_unlock, &client->screen_mutex);
            printf_now("<%s> [%s] %s",
                       str_time,
                       message.header.nickname,
                       ((char*) message.data) + (message.header.text_offset));
        pthread_cleanup_pop(true);
    pthread_cleanup_pop(true);
  }
}

int cli(client_t* client) {
  pthread_t receiver_thread;
  pthread_create(&receiver_thread, NULL, receiver_routine, client);

  while (true) {
    char* line_buf = NULL;
    size_t buf_size = 0;
    if (getline(&line_buf, &buf_size, stdin) < 0) {
      pthread_cancel(receiver_thread);
      if (errno == 0) {
        return 0;  // end-of-file
      } else {
        return -1;
      }
    }

    pthread_mutex_lock(&client->screen_mutex);
    pthread_cleanup_push(cleanup_mutex_unlock, &client->screen_mutex) ;
        if (strcmp(line_buf, ":m\n") != 0) {
          printf_now("Unknown command: %s", line_buf);
        } else {
          elegram_msg_t message;
          if (cli_get_message(&message, client->nickname) < 0) {
            if (errno == 0) {
              // EOF
              printf_now("\n");
              fflush(stdout);
            } else {
              perror("ERROR reading your message\n");
            }
          } else {
            send_message(client, &message);
          }
        }
    pthread_cleanup_pop(true);
  }
}

// Use EOF (C-d) to stop the client
int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "usage %s hostname nickname\n", argv[0]);
    exit(1);
  }

  const char* hostname = argv[1];
  const char* nickname = argv[2];

  if (strlen(nickname) > 31) {
    fprintf(stderr, "nickname should not be longer than 31 symbol\n");
    exit(1);
  }

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (sock_fd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  struct hostent* server = gethostbyname(hostname);

  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(ELEGRAM_SERVER_PORT),
      .sin_addr = *((struct in_addr*) server->h_addr),
  };

  /* Now connect to the server */
  if (connect(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting to the server");
    exit(1);
  }

  client_t client = {
      .screen_mutex = PTHREAD_MUTEX_INITIALIZER,
      .sock_fd = sock_fd,
  };
  strcpy(client.nickname, nickname);

  int ret = cli(&client);
  close(sock_fd);

  if (ret < 0) {
    perror("Oops");
    return 1;
  }

  return 0;
}

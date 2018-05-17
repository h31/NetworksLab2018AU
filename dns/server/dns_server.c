#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "dns_server.h"
#include "log.h"
#include "dns_socket_utils.h"


int dns_server_init(dns_server_t* server, uint16_t port) {
  LOG("Initializing server ...");

  *server = (dns_server_t) {
      .socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP),
  };

  if (server->socket < 0) {
    return -1;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_addr = {INADDR_ANY},
      .sin_port = htons(port),
  };

  if (bind(server->socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    return -1;
  }

  LOG("Server initialized");

  return 0;
}

void dns_server_destroy(dns_server_t* server) {
  close(server->socket);
}

static in_addr_t get_address(const char* name) {
  uint32_t result = 0;
  for (const char* cur = name; *cur != '\0'; ++cur) {
    result = result * 47 + (uint32_t) *cur;
  }

  return (in_addr_t) result;
}

int dns_server_serve(dns_server_t* server) {
  while (true) {
    dns_message_t request = {0};
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    dns_message_t reply = {0};

    LOG("Waiting for request ...");
    if (recv_dns_message(server->socket, &request,
                         (struct sockaddr*) &client_addr, &client_addr_len) < 0) {
      perror("Error reading request");
      goto next;
    }
    if (client_addr_len != sizeof(client_addr)) {
      fprintf(stderr, "Internal error: bad source address length\n");
      goto next;
    }
    LOG("Received request from %s:%d", inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port));

    if (request.header.question_count != 1) {
      fprintf(stderr, "Invalid request: expected question_count: 1, actual: %d\n",
              request.header.question_count);
      goto next;
    }

    if (!dns_message_is_request(&request)) {
      fprintf(stderr, "Invalid request");
      goto next;
    }

    reply = (dns_message_t) {
        .header = (dns_header_t) {
            .id = request.header.id,
            .control = DNS_CONTROL_REPLY,
            .question_count = 1,
            .answer_count = 1,
            .authority_count = 0,
            .additional_count = 0,
        },
        .question = new_dns_question(request.question->name),
        .answer = new_dns_answer(get_address(request.question->name)),
    };

    if (reply.question == NULL || reply.answer == NULL) {
      perror("Error creating reply");
      goto next;
    }

    if (send_dns_message(server->socket, &reply,
                         (struct sockaddr*) &client_addr, client_addr_len) < 0) {
      perror("Error sending reply");
      goto next;
    }

    LOG("Request successfully served [^_^]");

  next:
    dns_message_destroy(&reply);
    dns_message_destroy(&request);
  }
}

#include <stdio.h>
#include <arpa/inet.h>

#include "io_utils.h"
#include "log.h"
#include "dns_client.h"
#include "dns_socket_utils.h"


int dns_client_init(dns_client_t* client, const char* server, uint16_t port) {
  LOG("Initializing dns client ...");

  *client = (dns_client_t) {
      .next_id = 0,
      .socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP),
  };

  if (client->socket < 0) {
    return -1;
  }

  client->server_addr = (struct sockaddr_in) {
      .sin_family = AF_INET,
      .sin_port = htons(port),
  };

  if (inet_aton(server, &client->server_addr.sin_addr) == 0) {
    errno = EINVAL;
    return -1;
  }

  LOG("Dns client initialized");
  return 0;
}

void dns_client_destroy(dns_client_t* client) {
  close(client->socket);
  LOG("Dns client destroyed");
}

int dns_client_request(dns_client_t* client, const char* name, in_addr_t* result) {
  uint16_t request_id = client->next_id;
  client->next_id += 1;

  dns_message_t request = {
      .header = (dns_header_t) {
          .id = request_id,
          .control = DNS_CONTROL_RECURSIVE,
          .question_count = 1,
      },
      .question = new_dns_question(name),
      .answer = NULL,
  };
  dns_message_t reply = {0};

  if (request.question == NULL) {
    goto error;
  }

  LOG("Sending dns request ...");
  if (send_dns_message(client->socket, &request,
                       (struct sockaddr*) &client->server_addr,
                       sizeof(client->server_addr)) < 0) {
    goto error;
  }

  LOG("Dns request sent. Waiting for response ...");
  if (recv_dns_message(client->socket, &reply, NULL, NULL) < 0) {
    goto error;
  }
  LOG("Response received. Running checks ...");

  if (!dns_message_is_reply(&reply) || reply.header.id != request_id) {
    errno = EPROTO;
    goto error;
  }

  *result = reply.answer->address;
  LOG("Response is valid");

  int ret = 0;
end:
  dns_message_destroy(&request);
  dns_message_destroy(&reply);
  return ret;
error:
  ret = -1;
  goto end;
}

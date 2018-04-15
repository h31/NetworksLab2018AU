#ifndef AUDNS_DNS_PROTO_H
#define AUDNS_DNS_PROTO_H

#include <stdbool.h>
#include <netinet/in.h>
#include "byte_buf.h"

#define DNS_DEFAULT_PORT 53
#define DNS_MAX_PACKET_SIZE 1024

#define DNS_CONTROL_REPLY (1 << 15)
#define DNS_CONTROL_RECURSIVE (1 << 8)


typedef struct dns_header {
  uint16_t id;
  uint16_t control;

  uint16_t question_count;
  uint16_t answer_count;

  uint16_t authority_count;
  uint16_t additional_count;
} dns_header_t;

typedef struct dns_question {
  char name[512];
} dns_question_t;

dns_question_t* new_dns_question(const char* name);

static inline dns_question_t* new_empty_dns_question() {
  return calloc(1, sizeof(dns_question_t));
}

typedef struct dns_answer {
  in_addr_t address;
} dns_answer_t;

dns_answer_t* new_dns_answer(in_addr_t address);

static inline dns_answer_t* new_empty_dns_answer() {
  return calloc(1, sizeof(dns_answer_t));
}

typedef struct dns_message {
  dns_header_t header;
  dns_question_t* question;  // owning pointer
  dns_answer_t* answer;  // owning pointer
} dns_message_t;

void dns_message_destroy(dns_message_t* message);

int compile_dns_message(const dns_message_t* message, byte_buf_t* out);

int parse_dns_message(byte_buf_t* src, dns_message_t* out);

bool dns_message_is_reply(dns_message_t* message);

bool dns_message_is_request(dns_message_t* message);



#endif  // AUDNS_DNS_PROTO_H

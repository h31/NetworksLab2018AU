#include "dns_proto.h"

#include <assert.h>
#include <string.h>

#include "io_utils.h"
#include "log.h"


static const uint16_t OFFSET_TO_FIRST_NAME = 0xc00c;
static const uint32_t TTL_ONE_HOUR = 1 * 60 * 60;


dns_question_t* new_dns_question(const char* name) {
  dns_question_t* res = new_empty_dns_question();
  if (res == NULL) {
    return NULL;
  }

  strncpy(res->name, name, sizeof(res->name));
  if (res->name[sizeof(res->name) - 1] != '\0') {
    errno = E2BIG;
    free(res);
    return NULL;
  }

  return res;
}

dns_answer_t* new_dns_answer(in_addr_t address) {
  dns_answer_t* res = new_empty_dns_answer();
  if (res == NULL) {
    return NULL;
  }
  *res = (dns_answer_t) {.address = address};

  return res;
}

void dns_message_destroy(dns_message_t* message) {
  free(message->question);
  free(message->answer);
}

static ssize_t section_length(const char* section_start) {
  for (size_t i = 0; i < UINT8_MAX; ++i) {
    if (section_start[i] == '\0' || section_start[i] == '.') {
      return i;
    }
  }

  return -1;
}

static int compile_dns_header(byte_buf_t* buf, const dns_header_t* header) {
  if (byte_buf_write_short(buf, header->id) < 0
      || byte_buf_write_short(buf, header->control) < 0
      || byte_buf_write_short(buf, header->question_count) < 0
      || byte_buf_write_short(buf, header->answer_count) < 0
      || byte_buf_write_short(buf, header->authority_count) < 0
      || byte_buf_write_short(buf, header->additional_count) < 0) {
    return -1;
  }

  return 0;
}

static const char* compile_name_section(byte_buf_t* buf, const char* section_start) {
  ssize_t len = section_length(section_start);
  if (len < 0) {
    errno = E2BIG;
    return NULL;
  }

  uint8_t len_byte = (uint8_t) len;
  if (byte_buf_write(buf, &len_byte, 1) < 0) {
    return NULL;
  }
  if (byte_buf_write(buf, section_start, (size_t) len) < 0) {
    return NULL;
  }

  section_start += len;
  return section_start + ((section_start[0] != '\0') ? 1 : 0);
}

static int compile_dns_question(byte_buf_t* buf, const dns_question_t* question) {
  const char* section_start = question->name;
  while (section_start[0] != '\0') {
    section_start = compile_name_section(buf, section_start);
    if (section_start == NULL) {
      return -1;
    }
  }
  if (byte_buf_write_char(buf, 0) < 0) {
    return -1;
  }

  uint16_t qtype = 1, qclass = 1;
  if (byte_buf_write_short(buf, qtype) < 0 || byte_buf_write_short(buf, qclass) < 0) {
    return -1;
  }

  return 0;
}

static int compile_dns_answer(byte_buf_t* buf, const dns_answer_t* answer) {
  uint16_t name_offset = OFFSET_TO_FIRST_NAME,
      qtype = 1,
      qclass = 1,
      rd_length = 4;
  uint32_t time_to_live = TTL_ONE_HOUR;
  if (byte_buf_write_short(buf, name_offset) < 0
      || byte_buf_write_short(buf, qtype) < 0
      || byte_buf_write_short(buf, qclass) < 0
      || byte_buf_write_long(buf, time_to_live) < 0
      || byte_buf_write_short(buf, rd_length) < 0) {
    return -1;
  }

  if (byte_buf_write(buf, &answer->address, sizeof(answer->address)) < 0) {
    return -1;
  }

  return 0;
}

static int parse_dns_question(byte_buf_t* src, dns_question_t* question) {
  byte_buf_t name_buf = byte_buf_wrap(question->name, sizeof(question->name), 0);

  while (true) {
    uint8_t length;
    if (byte_buf_read_byte(src, &length) < 0) {
      return -1;
    }
    if (length == 0) {
      goto success;
    }
    if (byte_buf_copy(src, &name_buf, length) < 0 || byte_buf_write_char(&name_buf, '.') < 0) {
      return -1;
    }
  }

success:
  if (byte_buf_write_char(&name_buf, '\0') < 0) {
    return -1;
  }
  uint16_t qtype, qclass;
  if (byte_buf_read_short(src, &qtype) < 0 || byte_buf_read_short(src, &qclass) < 0) {
    return -1;
  }
  if (qtype != 1 || qclass != 1) {
    LOG("WARNING: message type %d or type %d is not supported", qtype, qclass);
    errno = EPROTO;
    return -1;
  }

  return 0;
}

int parse_dns_answer(byte_buf_t* src, dns_answer_t* answer) {
  uint16_t name_offset, qtype, qclass, rd_length;
  uint32_t time_to_live;
  if (byte_buf_read_short(src, &name_offset) < 0
      || byte_buf_read_short(src, &qtype) < 0
      || byte_buf_read_short(src, &qclass) < 0
      || byte_buf_read_long(src, &time_to_live) < 0
      || byte_buf_read_short(src, &rd_length) < 0) {
    return -1;
  }

  if (name_offset != OFFSET_TO_FIRST_NAME) {
    LOG("Protocol error: expected offset: %d, actual %d", OFFSET_TO_FIRST_NAME, name_offset);
    goto protocol_error;
  }

  if (qtype != 1 || qclass != 1) {
    LOG("Protocol error: message type %d or type %d is not supported", qtype, qclass);
    goto protocol_error;
  }

  if (rd_length != 4) {
    LOG("Protocol error: expected rd_length: 4, actual %d", rd_length);
    goto protocol_error;
  }

  if (byte_buf_read(src, &answer->address, sizeof(answer->address)) < 0) {
    return -1;
  }

  return 0;

protocol_error:
  errno = EPROTO;
  return -1;
}

int compile_dns_message(const dns_message_t* message, byte_buf_t* out) {
  if (compile_dns_header(out, &message->header) < 0
      || (message->question != NULL && compile_dns_question(out, message->question) < 0)
      || (message->answer != NULL && compile_dns_answer(out, message->answer) < 0)) {
    return -1;
  }
  return 0;
}

static int parse_header(byte_buf_t* src, dns_header_t* header) {
  if (byte_buf_read_short(src, &header->id) < 0
      || byte_buf_read_short(src, &header->control) < 0
      || byte_buf_read_short(src, &header->question_count) < 0
      || byte_buf_read_short(src, &header->answer_count) < 0
      || byte_buf_read_short(src, &header->authority_count) < 0
      || byte_buf_read_short(src, &header->additional_count) < 0) {
    return -1;
  }

  if (header->question_count > 1
      || header->answer_count > 1) {
    LOG("Protocol error: Only a single request and / or answer in a message is supported");
    errno = EPROTO;
    return -1;
  }

  if (header->authority_count > 0) {
    LOG("Warning: authorities are not supported");
  }

  if (header->additional_count > 0) {
    LOG("Warning: additional fields are not supported");
  }

  return 0;
}

int parse_dns_message(byte_buf_t* src, dns_message_t* out) {
  dns_message_t message = {0};

  if (parse_header(src, &message.header) < 0) {
    goto error;
  }

  assert(message.header.question_count <= 1);
  if (message.header.question_count == 1) {
    message.question = new_empty_dns_question();
    if (message.question == NULL) {
      goto error;
    }
  }

  assert(message.header.answer_count <= 1);
  if (message.header.answer_count == 1) {
    message.answer = new_empty_dns_answer();
    if (message.answer == NULL) {
      goto error;
    }
  }

  if ((message.header.question_count == 1 && parse_dns_question(src, message.question) < 0)
      || (message.header.answer_count == 1 && parse_dns_answer(src, message.answer) < 0)) {
    goto error;
  }

  *out = message;
  return 0;
error:
  dns_message_destroy(&message);
  return -1;
}

bool dns_message_is_reply(dns_message_t* message) {
  return (message->header.control & DNS_CONTROL_REPLY) != 0;
}

bool dns_message_is_request(dns_message_t* message) {
  return !dns_message_is_reply(message);
}

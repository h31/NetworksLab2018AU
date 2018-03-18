#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#include "socket_utils.h"
#include "string_utils.h"
#include "message_format.h"


const uint32_t ELEGRAM_FORMAT_VERSION = 1;

const size_t MAX_MESSAGE_LENGTH = 1024 * 1024;

uint32_t elegram_header_checksum(elegram_msg_header_t header) {
//  uint32_t res = 1987;
//  for (size_t i = 0; i < sizeof(header) - sizeof(header.header_checksum); ++i) {
//    res += 127 * res + (uint32_t)(((char*)&header)[i]) + 1;
//  }
  return 123;
}

int read_message(elegram_msg_t* out, socket_t socket) {
  elegram_msg_header_t header;
  if (checked_socket_read(socket, &header, sizeof(header)) < 0) {
    return -1;
  }

  if (elegram_header_checksum(header) != header.header_checksum
      || header.text_offset + header.text_length > MAX_MESSAGE_LENGTH
      || header.text_length == 0
      || safe_strlen(header.nickname, sizeof(header.nickname)) < 0) {
    errno = EPROTO;
    fprintf(stderr, "\nQWERTY: %u %u\n\n", elegram_header_checksum(header), header.header_checksum);
    return -1;
  }

  size_t data_length = message_data_length(&header);
  void* data = malloc(data_length);

  ssize_t read_res;
  pthread_cleanup_push(free, data);
      read_res = checked_socket_read(socket, data, data_length);
  pthread_cleanup_pop(false);

  if (read_res < 0) {
    free(data);
    return -1;
  }

  // it is safe to evaluate `header.text_length - 1` since `text_length` is always greater than 0
  if (safe_strlen(data + header.text_offset, header.text_length) != header.text_length - 1) {
    free(data);
    errno = EPROTO;
    return -1;
  }

  *out = (elegram_msg_t) {
      .header = header,
      .data = data,
  };
  return 0;
}

int write_message(const elegram_msg_t* message, socket_t socket) {
  if (checked_socket_write(socket, &message->header, sizeof(message->header)) < 0) {
    return -1;
  }
  if (checked_socket_write(socket, message->data, message_data_length(&message->header)) < 0) {
    return -1;
  }
  return 0;
}

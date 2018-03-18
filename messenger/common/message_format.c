#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

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

static ssize_t checked_read(int fd, void* buf, size_t buf_size) {
  ssize_t read_res = read(fd, buf, buf_size);
  if (read_res < 0) {
    return -1;
  }
  if (read_res != buf_size) {
    errno = EIO;
    return -1;
  }
  return read_res;
}

static ssize_t checked_write(int fd, const void* buf, size_t buf_size) {
  ssize_t write_res = write(fd, buf, buf_size);
  if (write_res < 0) {
    return -1;
  }
  if (write_res != buf_size) {
    errno = EIO;
    return -1;
  }
  return write_res;
}

int read_message(elegram_msg_t* out, int sock_fd) {
  elegram_msg_header_t header;
  if (checked_read(sock_fd, &header, sizeof(header)) < 0) {
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
      read_res = checked_read(sock_fd, data, data_length);
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

int write_message(const elegram_msg_t* message, int sock_fd) {
  if (checked_write(sock_fd, &message->header, sizeof(message->header)) < 0) {
    return -1;
  }
  if (checked_write(sock_fd, message->data, message_data_length(&message->header)) < 0) {
    return -1;
  }
  return 0;
}

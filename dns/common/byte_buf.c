#include "byte_buf.h"

#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <byte_buf.h>
#include <assert.h>


static inline int check_capacity(const byte_buf_t* buf, size_t count) {
  return (buf->position + count > buf->capacity) ? (errno = EOVERFLOW, -1) : 0;
}

static inline int check_read_mode_value(const byte_buf_t* buf, bool expected) {
  return (buf->read_mode != expected) ? (errno = EINVAL, -1) : 0;
}

static inline int check_read_mode(const byte_buf_t* buf) {
  return check_read_mode_value(buf, true);
}

static inline int check_write_mode(const byte_buf_t* buf) {
  return check_read_mode_value(buf, false);
}

static inline int check_write(const byte_buf_t* buf, size_t count) {
  return (check_capacity(buf, count) < 0 || check_write_mode(buf) < 0) ? -1 : 0;
}

static inline int check_read(const byte_buf_t* buf, size_t count) {
  return (check_capacity(buf, count) < 0 || check_read_mode(buf) < 0) ? -1 : 0;
}

void* byte_buf_tail(byte_buf_t* buf) {
  return (char*) buf->data + buf->position;
}

size_t byte_buf_tail_length(byte_buf_t* buf) {
  assert(buf->position <= buf->capacity);
  return buf->capacity - buf->position;
}

static inline void* tail_and_inc(byte_buf_t* buf, size_t count) {
  assert(check_capacity(buf, count) == 0);
  void* res = byte_buf_tail(buf);
  buf->position += count;
  return res;
}

byte_buf_t byte_buf_wrap(void* data, size_t capacity, size_t position) {
  return (byte_buf_t) {
      .data = data,
      .max_capacity = capacity,
      .capacity = capacity,
      .position = position,
      .read_mode = false,
  };
}

int byte_buf_start_read(byte_buf_t* buf) {
  if (check_write_mode(buf)) {
    return -1;
  }
  buf->capacity = buf->position;
  buf->position = 0;
  buf->read_mode = true;
  return 0;
}

int byte_buf_copy(byte_buf_t* src, byte_buf_t* dst, size_t count) {
  if (check_read(src, count) < 0 || check_write(dst, count) < 0) {
    return -1;
  }
  memcpy(tail_and_inc(dst, count), tail_and_inc(src, count), count);
  return 0;
}

int byte_buf_write(byte_buf_t* dst, const void* src, size_t count) {
  if (check_write(dst, count) < 0) {
    return -1;
  }

  memcpy(tail_and_inc(dst, count), src, count);
  return 0;
}

int byte_buf_write_long(byte_buf_t* dst, uint32_t val) {
  uint32_t val_be = htonl(val);
  return byte_buf_write(dst, &val_be, sizeof(val_be));
}

int byte_buf_write_short(byte_buf_t* dst, uint16_t val) {
  uint16_t val_be = htons(val);
  return byte_buf_write(dst, &val_be, sizeof(val_be));
}

int byte_buf_write_char(byte_buf_t* dst, char ch) {
  return byte_buf_write(dst, &ch, 1);
}


int byte_buf_read(byte_buf_t* src, void* dst, size_t count) {
  if (check_read(src, count) < 0) {
    return -1;
  }

  memcpy(dst, tail_and_inc(src, count), count);
  return 0;
}

int byte_buf_read_long(byte_buf_t* src, uint32_t* dst) {
  uint32_t value;
  if (byte_buf_read(src, &value, sizeof(value)) < 0) {
    return -1;
  }

  *dst = ntohl(value);
  return 0;
}

int byte_buf_read_short(byte_buf_t* src, uint16_t* dst) {
  uint16_t value;
  if (byte_buf_read(src, &value, sizeof(value)) < 0) {
    return -1;
  }

  *dst = ntohs(value);
  return 0;
}

int byte_buf_read_byte(byte_buf_t* src, uint8_t* dst) {
  return byte_buf_read(src, dst, 1);
}

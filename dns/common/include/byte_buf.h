#ifndef AUDNS_BYTE_BUF_H
#define AUDNS_BYTE_BUF_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  void* data;
  size_t max_capacity;
  size_t capacity;
  size_t position;
  bool read_mode;
} byte_buf_t;


byte_buf_t byte_buf_wrap(void* data, size_t capacity, size_t position);

int byte_buf_start_read(byte_buf_t* buf);

void* byte_buf_tail(byte_buf_t* buf);

size_t byte_buf_tail_length(byte_buf_t* buf);


// Write mode

int byte_buf_copy(byte_buf_t* src, byte_buf_t* dst, size_t count);

int byte_buf_write(byte_buf_t* dst, const void* src, size_t count);

int byte_buf_write_long(byte_buf_t* dst, uint32_t val);

int byte_buf_write_short(byte_buf_t* dst, uint16_t val);

int byte_buf_write_char(byte_buf_t* dst, char ch);


// Read mode

int byte_buf_read(byte_buf_t* src, void* dst, size_t count);

int byte_buf_read_long(byte_buf_t* src, uint32_t* dst);

int byte_buf_read_short(byte_buf_t* src, uint16_t* dst);

int byte_buf_read_byte(byte_buf_t* src, uint8_t* dst);

#endif  // AUDNS_BYTE_BUF_H

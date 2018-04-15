#ifndef AUDNS_IO_UTILS_H
#define AUDNS_IO_UTILS_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <netinet/in.h>


static inline int checked_write(int fd, const void* buf, size_t count) {
  ssize_t write_res = write(fd, buf, count);

  if (write_res < 0) {
    return -1;
  }
  if ((size_t) write_res != count) {
    errno = EIO;
    return -1;
  }
  return 0;
}

static inline int checked_read(int fd, void* buf, size_t count) {
  ssize_t read_res = read(fd, buf, count);

  if (read_res < 0) {
    return -1;
  }
  if ((size_t) read_res != count) {
    errno = EIO;
    return -1;
  }
  return 0;
}

static inline int parse_u16(const char* s, uint16_t* out) {
  errno = 0;
  long raw = strtol(s, NULL, 10);
  if (errno != 0) {
    return -1;
  }
  if (raw < 0 || raw > UINT16_MAX) {
    errno = EOVERFLOW;
    return -1;
  }
  *out = (uint16_t) raw;
  return 0;
}

#endif  // AUDNS_IO_UTILS_H

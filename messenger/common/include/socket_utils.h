#ifndef ELEGRAM_SOCKET_UTILS_H
#define ELEGRAM_SOCKET_UTILS_H

#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "ssize_t.h"


#ifndef EPROTO
  #define EPROTO 1
#endif

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

static inline int socket_utils_init() {
#ifdef _WIN32
  WSADATA wsdata;
  if (WSAStartup(0x0101, &wsdata) != 0) {
    errno = EIO;
    return -1;
  }
#endif
  return 0;
}

static inline int socket_utils_cleanup() {
#ifdef _WIN32
  if (WSACleanup() != 0) {
    errno = EIO;
    return -1;
  }
#endif
  return 0;
}

static inline void close_socket(socket_t socket) {
#ifdef _WIN32
  closesocket(socket);
#else
  close(socket);
#endif
}

/**
 * Returns 0 on success, and -1 on failure
 */
static inline ssize_t checked_socket_read(socket_t socket, void* buf, size_t buf_size) {
  #ifdef _WIN32
  ssize_t read_res = recv(socket, (char*) buf, buf_size, 0);
  #else
  ssize_t read_res = read(socket, buf, buf_size);
  #endif

  if (read_res < 0) {
    #ifdef _WIN32
    errno = EIO;
    #endif
    return -1;
  }
  if (read_res != buf_size) {
    errno = EIO;
    return -1;
  }
  return read_res;
}

static inline ssize_t checked_socket_write(socket_t socket, const void* buf, size_t buf_size) {
  #ifdef _WIN32
  ssize_t write_res = send(socket, (const char*) buf, buf_size, 0);
  #else
  ssize_t write_res = write(socket, buf, buf_size);
  #endif

  if (write_res < 0) {
    #ifdef _WIN32
    errno = EIO;
    #endif
    return -1;
  }
  if (write_res != buf_size) {
    errno = EIO;
    return -1;
  }
  return write_res;
}

static inline socket_t create_tcp_socket() {
  return socket(AF_INET, SOCK_STREAM, 0);
}


#endif  // ELEGRAM_SOCKET_UTILS_H

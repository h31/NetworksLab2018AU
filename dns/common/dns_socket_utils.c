#include <sys/socket.h>

#include "byte_buf.h"
#include "dns_socket_utils.h"


int send_dns_message(int fd, const dns_message_t* message,
                     struct sockaddr* dest_addr, socklen_t addr_len) {
  char data[DNS_MAX_PACKET_SIZE];
  byte_buf_t buf = byte_buf_wrap(data, sizeof(data), 0);

  if (compile_dns_message(message, &buf) < 0) {
    return -1;
  }

  if (sendto(fd, buf.data, buf.position, 0, dest_addr, addr_len) < 0) {
    return -1;
  }

  return 0;
}

int recv_dns_message(int fd, dns_message_t* out,
                     struct sockaddr* addr, socklen_t* addr_len) {
  char data[DNS_MAX_PACKET_SIZE];

  ssize_t buf_length = recvfrom(fd, data, sizeof(data), 0, addr, addr_len);
  if (buf_length < 0) {
    return -1;
  }

  byte_buf_t buf = byte_buf_wrap(data, sizeof(data), (size_t) buf_length);
  if (byte_buf_start_read(&buf) < 0 || parse_dns_message(&buf, out) < 0) {
    return -1;
  }

  return 0;
}

#ifndef AUDNS_DNS_SOCKET_UTILS_H
#define AUDNS_DNS_SOCKET_UTILS_H

#include "dns_proto.h"


int send_dns_message(int fd, const dns_message_t* message,
                     struct sockaddr* dest_addr, socklen_t addr_len);

int recv_dns_message(int fd, dns_message_t* out,
                     struct sockaddr* addr, socklen_t* addr_len);

#endif  // AUDNS_DNS_SOCKET_UTILS_H

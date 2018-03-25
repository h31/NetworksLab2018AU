#pragma once

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>

#define SOCKET int
#define POLLFD pollfd
#define ADDRINFOA addrinfo

#define DNS_PORT 53
#define HTTP_PORT 80
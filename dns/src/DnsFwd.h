#ifndef DNS_FWD_H
#define DNS_FWD_H

#include <memory>
#include <cstdint>

#include <netinet/in.h>

struct DnsSocket;
using DnsSocketWrapper = std::shared_ptr<DnsSocket>;

#define DEBUG_DNS 0

#define DEFAULT_UDP_PORT 53

#endif //DNS_FWD_H

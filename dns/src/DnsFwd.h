#ifndef MESSENGER_ELEGRAMFWD_H
#define MESSENGER_ELEGRAMFWD_H

#include <memory>
#include <cstdint>

struct DnsSocket;
using DnsSocketWrapper = std::shared_ptr<DnsSocket>;

#endif //MESSENGER_ELEGRAMFWD_H

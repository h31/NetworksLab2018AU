#ifndef MESSENGER_ELEGRAMFWD_H
#define MESSENGER_ELEGRAMFWD_H

#include <memory>
#include <cstdint>

#include "MessageType.h"

struct ServerSocket;
using ServerSocketWrapper = std::shared_ptr<ServerSocket>;

struct Socket;
using SocketWrapper = std::shared_ptr<Socket>;

struct DnsSocket;
using DnsSocketWrapper = std::shared_ptr<DnsSocket>;

#endif //MESSENGER_ELEGRAMFWD_H

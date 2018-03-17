#ifndef MESSENGER_ELEGRAMFWD_H
#define MESSENGER_ELEGRAMFWD_H

#include <memory>
#include <cstdint>

#include "MessageType.h"

struct ServerSocket;
using ServerSocketWrapper = std::shared_ptr<ServerSocket>;

struct Socket;
using SocketWrapper = std::shared_ptr<Socket>;

struct Message;

#if _WIN32
using ssize_t = std::int64_t;
#endif

#endif //MESSENGER_ELEGRAMFWD_H

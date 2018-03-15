#ifndef MESSENGER_ELEGRAMFWD_H
#define MESSENGER_ELEGRAMFWD_H

#include <memory>

#include "MessageType.h"

struct ServerSocket;
using ServerSocketWrapper = std::shared_ptr<ServerSocket>;

struct Socket;
using SocketWrapper = std::shared_ptr<Socket>;

struct Message;

#endif //MESSENGER_ELEGRAMFWD_H

#ifndef MESSENGER_ELEGRAMFWD_H
#define MESSENGER_ELEGRAMFWD_H

#include <memory>

struct ServerSocket;
using ServerSocketWrapper = std::shared_ptr<ServerSocket>;

struct Socket;
using SocketWrapper = std::shared_ptr<Socket>;

struct Message;
using MessageWrapper = std::shared_ptr<Message>;

#endif //MESSENGER_ELEGRAMFWD_H

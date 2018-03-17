#pragma once

#include <string>
#include "Message.h"

struct Socket {
    Socket(const std::string &host, uint16_t port);
    ~Socket();
    Message read();
    void write(const Message & message);

private:
    const std::string host;
    const uint16_t port;
    const int socketDescriptor;
};



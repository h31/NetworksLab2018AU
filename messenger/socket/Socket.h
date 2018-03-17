#pragma once

#include <string>
#include "Message.h"

struct Socket {
    Socket();
    explicit Socket(int socketDescriptor);
    Socket(Socket && other) noexcept;
    Socket(const Socket & other) = default;
    virtual ~Socket();
    Message read();
    void write(const Message & message);

protected:
    int socketDescriptor;
};



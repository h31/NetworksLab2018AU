#pragma once

#include <string>
#include "Message.h"

struct FailedToReadMessageException : std::exception {
    const char *what() const noexcept override;
};

struct Socket {
    Socket();
    explicit Socket(int socketDescriptor);
    Socket(Socket && other) noexcept;
    Socket(const Socket & other) = default;
    virtual ~Socket();
    Message read();
    void write(const Message & message);
    bool interesting();
    void close();

protected:
    int socketDescriptor;
    bool dead;
};



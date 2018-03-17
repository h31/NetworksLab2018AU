#pragma once

#include <string>
#include "Message.h"

struct Socket {
    Socket();
    explicit Socket(int socketDescriptor);
    virtual ~Socket();
    Message read();
    void write(const Message & message);

protected:
    const int socketDescriptor;
};



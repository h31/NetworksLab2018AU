#pragma once

#include <string>
#include "Message.h"

struct Socket {
    Socket(const std::string &host, int port);
    Message read();
    void write(const Message & message);

private:
    std::string host;
    int port;
};



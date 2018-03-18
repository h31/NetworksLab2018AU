#pragma once


#include <Socket.h>

struct ClientSocket : Socket {
    ClientSocket(const std::string &host, uint16_t port);
};



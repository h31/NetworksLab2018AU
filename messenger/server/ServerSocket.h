#pragma once


#include <Socket.h>

struct ServerSocket : Socket {
    explicit ServerSocket(uint16_t port);
    Socket accept();
};



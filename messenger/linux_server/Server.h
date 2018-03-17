#pragma once

#include "ServerSocket.h"

#include <cstdint>
#include <thread>

struct Server {
    explicit Server(uint16_t port);
    ~Server();


private:
    void serverRoutine();
    std::thread server;
    ServerSocket serverSocket;
};



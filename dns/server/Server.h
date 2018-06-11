#pragma once


#include <cstdint>
#include <thread>
#include <atomic>

struct Server {
    explicit Server(uint16_t serverPort);

    ~Server();

    void start();

    void stop();

private:
    void initSocket();

    void serveClient();

    uint16_t serverPort;
    int socketDescriptor;
    std::atomic_bool stopped;
    std::thread worker;
};



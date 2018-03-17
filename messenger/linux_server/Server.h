#pragma once

#include "ServerSocket.h"

#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <queue>
#include <condition_variable>

struct Server {
    explicit Server(uint16_t port);
    ~Server();

private:
    using guard = std::lock_guard<std::mutex>;
    using unique_lock = std::unique_lock<std::mutex>;

    void acceptorRoutine();
    void senderRoutine();
    void servingRoutine(Socket socket);

    struct ClientHandle {
        ClientHandle(std::thread && thread, Socket && socket);
        std::thread worker;
        Socket socket;
    };

    std::atomic_bool stopped;
    ServerSocket serverSocket;
    std::thread acceptor;
    std::thread sender;
    std::vector<ClientHandle> clientHandles;
    std::queue<Message> pendingMessages;
    std::mutex queueLock;
    std::condition_variable queueCond;
};



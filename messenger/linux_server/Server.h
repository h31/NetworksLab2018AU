#pragma once

#include "ServerSocket.h"

#include <cstdint>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <queue>
#include <condition_variable>

struct Server {
    explicit Server(uint16_t port);
    ~Server();

private:
    using guard = std::lock_guard<std::mutex>;
    using unique_lock = std::unique_lock<std::mutex>;
    using SocketPtr = std::shared_ptr<Socket>;

    void acceptorRoutine();
    void senderRoutine();
    void servingRoutine(SocketPtr socket);

    void insertClient(const SocketPtr & socket);

    struct ClientHandle {
        ClientHandle(std::thread && thread, const SocketPtr &socket);
        ClientHandle(ClientHandle && other) noexcept;
        ClientHandle(const ClientHandle & other) = delete;

        std::thread worker;
        SocketPtr socket;
    };

    std::atomic_bool stopped;
    ServerSocket serverSocket;
    std::thread acceptor;
    std::thread sender;
    std::unordered_map<int, ClientHandle> clientHandles;
    std::unordered_set<int> deadHandles;
    std::queue<Message> pendingMessages;
    std::mutex lock;
    std::condition_variable cond;
};



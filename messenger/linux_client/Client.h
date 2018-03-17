#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <set>
#include "Socket.h"


struct Client {
    Client(const std::string &address, uint16_t port, const std::string & nickname);

    void mute();
    void unmute();
    void send(const std::string & message);

private:
    using guard = std::lock_guard<std::mutex>;
    using unique_lock = std::unique_lock<std::mutex>;

    void readerRoutine();
    void printerRoutine();
    Socket socket;
    std::thread reader;
    std::thread printer;
    const std::string nickname;
    std::multiset<Message> messageQueue;
    std::atomic_bool muted;
    std::atomic_bool stopped;
    std::mutex queueLock;
    std::condition_variable queueCond;
};

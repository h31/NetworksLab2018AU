#include <iostream>
#include "Server.h"

Server::Server(uint16_t port) :
        stopped(false),
        serverSocket(port),
        acceptor(&Server::acceptorRoutine, this),
        sender(&Server::senderRoutine, this)
{}

Server::~Server() {
    stopped = true;
    if (acceptor.joinable()) {
        acceptor.join();
    }

    if (sender.joinable()) {
        sender.join();
    }

    for (ClientHandle & handle: clientHandles) {
        if (handle.worker.joinable()) {
            handle.worker.join();
        }
    }
}

void Server::acceptorRoutine() {
    while (!stopped) {
        Socket client = serverSocket.accept();
        std::thread worker(&Server::servingRoutine, this, client);
        clientHandles.emplace_back(std::move(worker), std::move(client));
    }
}

void Server::senderRoutine() {
    while (!stopped) {
        unique_lock lk(queueLock);
        queueCond.wait(lk, [this](){return !pendingMessages.empty();});
        while (!pendingMessages.empty()) {
            Message & msg = pendingMessages.front();
            for (ClientHandle & handle : clientHandles) {
                handle.socket.write(msg);
            }
            pendingMessages.pop();
        }
        lk.unlock();
    }
}

void Server::servingRoutine(Socket socket) {
    while (!stopped) {
        Message msg = socket.read();
        msg.time = "12:04";
        guard lk(queueLock);
        pendingMessages.push(msg);
        queueCond.notify_one();
    }
}

Server::ClientHandle::ClientHandle(std::thread &&thread, Socket &&socket) : worker(std::move(thread)), socket(std::move(socket)) {}

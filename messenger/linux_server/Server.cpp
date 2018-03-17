#include <iostream>
#include <csignal>
#include "Server.h"

Server::Server(uint16_t port) :
        stopped(false),
        serverSocket(port),
        acceptor(&Server::acceptorRoutine, this),
        sender(&Server::senderRoutine, this)
{}

Server::ClientHandle::ClientHandle(std::thread &&thread, Socket &&socket) : worker(std::move(thread)), socket(std::move(socket)) {}

Server::~Server() {
    std::cout << "server destructor called" << std::endl;
    stopped = true;

    serverSocket.close();
    if (acceptor.joinable()) {
        acceptor.join();
    }
    std::cout << "acceptor thread joined" << std::endl;


    queueCond.notify_one();
    if (sender.joinable()) {
        sender.join();
    }
    std::cout << "sender thread joined" << std::endl;

    for (ClientHandle & handle: clientHandles) {
        handle.socket.close();
        if (handle.worker.joinable()) {
            handle.worker.join();
            std::cout << "worker thread joined" << std::endl;
        }
    }
}

void Server::acceptorRoutine() {
    while (!stopped) {
        if (serverSocket.interesting()) {
            std::cout << "have client to accept!" << std::endl;
            Socket client = serverSocket.accept();
            std::cout << "client accepted!" << std::endl;
            std::thread worker(&Server::servingRoutine, this, client);
            clientHandles.emplace_back(std::move(worker), std::move(client));
        } else {

        }
    }
    std::cout << "acceptor thread finished" << std::endl;
}

void Server::servingRoutine(Socket socket) {
    while (!stopped) {
        if (socket.interesting()) {
            try {
                Message msg = socket.read();
                std::cout << "message received!" << std::endl;
                msg.time = "12:04";
                guard lk(queueLock);
                pendingMessages.push(msg);
                queueCond.notify_one();
            } catch (const FailedToReadMessageException & ex) {
                std::cout << "catched FailedToReadMessageException" << std::endl;
                break;
            }
        } else {
            std::this_thread::yield();
        }
    }
    std::cout << "serving thread finished" << std::endl;
}

void Server::senderRoutine() {
    while (!stopped) {
        unique_lock lk(queueLock);
        queueCond.wait(lk);
        while (!pendingMessages.empty()) {
            Message & msg = pendingMessages.front();
            for (ClientHandle & handle : clientHandles) {
                handle.socket.write(msg);
                std::cout << "message sent to client!" << std::endl;
            }
            pendingMessages.pop();
        }
        lk.unlock();
    }
    std::cout << "sender thread finished" << std::endl;
}

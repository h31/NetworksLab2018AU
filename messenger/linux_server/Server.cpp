#include <iostream>
#include <csignal>
#include <iomanip>
#include <sstream>
#include "Server.h"

Server::Server(uint16_t port) :
        stopped(false),
        serverSocket(port),
        acceptor(&Server::acceptorRoutine, this),
        sender(&Server::senderRoutine, this)
{}

Server::ClientHandle::ClientHandle(std::thread &&thread, const SocketPtr &socket) : worker(std::move(thread)), socket(socket) {}

Server::ClientHandle::ClientHandle(Server::ClientHandle &&other) noexcept : worker(std::move(other.worker)), socket(std::move(other.socket)) {}

Server::~Server() {
    std::cout << "server destructor called" << std::endl;
    stopped = true;

    serverSocket.close();
    if (acceptor.joinable()) {
        acceptor.join();
    }
    std::cout << "acceptor thread joined" << std::endl;

    cond.notify_one();
    if (sender.joinable()) {
        sender.join();
    }
    std::cout << "sender thread joined" << std::endl;

    guard lk(lock);
    for (auto & p : clientHandles) {
        ClientHandle & handle = p.second;
        handle.socket->close();
        if (handle.worker.joinable()) {
            handle.worker.join();
            std::cout << "worker thread joined" << std::endl;
        }
    }
}

void Server::insertClient(const Server::SocketPtr &socket) {
    int clientDescriptor = socket->descriptor();
    guard lk(lock);
    for (int id : deadHandles) {
        ClientHandle &handle = clientHandles.find(id)->second;
        if (handle.worker.joinable()) {
            handle.worker.join();
        }
        clientHandles.erase(id);
    }
    deadHandles.clear();

    if (clientHandles.find(clientDescriptor) != clientHandles.end()) {
        throw std::runtime_error("ERROR: trying to accept client with already existing socket");
    }
    std::thread worker(&Server::servingRoutine, this, socket);
    clientHandles.insert(std::make_pair(clientDescriptor, ClientHandle{std::move(worker), socket}));
}

void Server::acceptorRoutine() {
    while (!stopped) {
        if (serverSocket.interesting()) {
            std::shared_ptr<Socket> client = std::make_shared<Socket>(serverSocket.accept());
            insertClient(client);
        } else {
            std::this_thread::yield();
        }
    }
    std::cout << "acceptor thread finished" << std::endl;
}

std::string getTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%H:%M");
    std::string result = ss.str();
    return result;
}

void Server::servingRoutine(SocketPtr socket) {
    while (!stopped) {
        if (socket->interesting()) {
            try {
                Message msg = socket->read();
                msg.time = getTime();
                guard lk(lock);
                pendingMessages.push(msg);
                cond.notify_one();
            } catch (const SocketException & ex) {
				(void) ex;
                std::cout << "catched FailedToReadMessageException" << std::endl;
                break;
            }
        } else {
            std::this_thread::yield();
        }
    }
    guard lk(lock);
    deadHandles.insert(socket->descriptor());
    std::cout << "serving thread finished" << std::endl;
}

void Server::senderRoutine() {
    while (!stopped) {
        unique_lock lk(lock);
        cond.wait(lk);
        while (!pendingMessages.empty()) {
            Message & msg = pendingMessages.front();
            for (auto & p : clientHandles) {
                int descriptor = p.first;
                ClientHandle & handle = p.second;
                if (deadHandles.find(descriptor) == deadHandles.end()) {
                    handle.socket->write(msg);
                }
            }
            pendingMessages.pop();
        }
        lk.unlock();
    }
    std::cout << "sender thread finished" << std::endl;
}

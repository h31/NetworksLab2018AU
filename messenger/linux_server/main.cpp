#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <cstring>
#include <cstddef>
#include <limits>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cassert>
#include "ElegramAll.h"

volatile bool is_finished = false;

//static std::vector<pthread_t> acceptor_threads;

struct SocketStorage {
    explicit SocketStorage(SocketWrapper socket)
            : socket(std::move(socket))
    {}
    
    SocketWrapper socket;
    std::mutex mutex;
//    std::priority_queue<Message, std::less<Message>> message_queue;
};

using SocketStorageWrapper = std::shared_ptr<SocketStorage>;

std::list<SocketStorageWrapper> sockets;
std::mutex sockets_mutex;

std::vector<std::unique_ptr<std::thread>> threads;

// TODO acceptor mutex.

static void print_message(const Message &message) {
    static std::mutex print_mutex;
    std::unique_lock<std::mutex> lock(print_mutex);
    std::cout << message << std::endl;
    lock.unlock();
    std::unique_lock<std::mutex> sockets_lock(sockets_mutex);
    for (auto ssit = sockets.begin(); ssit != sockets.end(); ) {
        if (ssit->unique()) {
            ssit = sockets.erase(ssit);
        } else {
            // TODO priority queue.
            std::unique_lock<std::mutex> socket_lock((*ssit)->mutex);
            auto &socket = (*ssit)->socket;
            socket->write_broadcast(message);
            socket_lock.unlock();
            ++ssit;
        }
    }
}

static void client_session(SocketStorageWrapper socketStorage) {
    auto &acceptSocket = socketStorage->socket;
    std::mutex &mutex = socketStorage->mutex;
    while (!is_finished) {
        std::unique_lock<std::mutex> lock(mutex);
        auto message_type = acceptSocket->read_uint();
        if (message_type == static_cast<uint32_t>(MessageType::FINISH)) {
            break;
        }
        auto message = acceptSocket->read_message();
        lock.unlock();
        print_message(message);
    }
    std::cout << "Finished session with client: " << acceptSocket->other_username << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " port" << std::endl;
        exit(1);
    }
    auto const port = stoi(static_cast<std::string>(argv[1]));
    auto serverSocket = std::make_shared<ServerSocket>(port);
    serverSocket->listen();
    while (!is_finished) {
        auto acceptSocket = serverSocket->accept();
        auto sockets_storage = std::make_shared<SocketStorage>(acceptSocket);
        std::unique_lock<std::mutex> sockets_lock{sockets_mutex};
        sockets.push_back(sockets_storage);
        threads.emplace_back(new std::thread(client_session, sockets_storage));
    }
    return 0;
}

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

std::list<SocketWrapper> sockets;
std::mutex sockets_mutex;
static std::mutex print_mutex;
template<typename T>
static void print_stdout(const T &arg) {
    std::unique_lock<std::mutex> lock(print_mutex);
    std::cout << arg << std::endl;
}

// broadcast_message is enterable by several client_session threads.
static void broadcast_message(const Message &message) {
    std::unique_lock<std::mutex> sockets_lock(sockets_mutex);
    // TODO lock free list.
    print_stdout(message);
    for (auto ssit = sockets.begin(); ssit != sockets.end(); ) {
        if (ssit->unique()) {
            ssit = sockets.erase(ssit);
        } else {
            auto &socket = *ssit;
            socket->write_broadcast(message);
            ++ssit;
        }
    }
}

static void client_session(const SocketWrapper &acceptSocket) {
    while (!is_finished) {
        auto message_type = acceptSocket->read_uint();
        if (message_type == static_cast<uint32_t>(MessageType::FINISH)) {
            break;
        }
        auto message = acceptSocket->read_message();
        // TODO priority queue.
        broadcast_message(message);
    }
    std::cout << "Finished session with client: " << acceptSocket->other_username << std::endl;
    acceptSocket->finish();
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
        std::unique_lock<std::mutex> sockets_lock{sockets_mutex};
        sockets.push_back(acceptSocket);
        std::thread client_thread{client_session, acceptSocket};
        client_thread.detach();
    }
    return 0;
}

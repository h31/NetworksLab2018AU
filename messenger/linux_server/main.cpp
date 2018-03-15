#include <iostream>
#include <vector>
#include <cstring>
#include <cstddef>
#include <limits>
#include <algorithm>
#include <cassert>
#include <pthread.h>
#include "ElegramAll.h"

volatile bool is_finished = false;

static std::vector<pthread_t> acceptor_threads;

static void *client_session(void *data) {
    auto acceptSocket = reinterpret_cast<Socket *>(data);
    while (!is_finished) {
        auto message_type = acceptSocket->read_uint();
        if (message_type == static_cast<uint32_t>(MessageType::FINISH)) {
            break;
        }
        auto message = acceptSocket->read_message();
        
        std::cout << "Here is date: " << message->date.pretty_string()
                  << ", Here is the message: " + message->buffer
                  << ", Here is the username: " + message->username
                  << std::endl;
        // TODO full address.
        acceptSocket->write_message("I got your message: " + message->buffer, Date::now());
    }
    std::cout << "Finished session with client: " << acceptSocket->other_username << std::endl;
    delete acceptSocket;
    return nullptr;
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
        pthread_t next_thread;
        pthread_create(&next_thread, nullptr, client_session, acceptSocket);
        acceptor_threads.push_back(next_thread);
        pthread_join(next_thread, nullptr);
    }
    return 0;
}

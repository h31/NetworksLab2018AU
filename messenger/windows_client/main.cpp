#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cassert>

#include "ElegramAll.h"

static SocketWrapper clientSocket;
std::mutex socket_mutex;
static auto last_message = Message::invalid_message();
static std::mutex print_mutex;

volatile bool is_finished = false;

void broadcast_callback() {
    while (!is_finished) {
        std::uint32_t itype;
        try {
            itype = clientSocket->read_uint();
        }
        catch (std::exception &e) {
            if (!is_finished) { // otherwise it's closed.
                std::cerr << "Error while reading broadcast: " << e.what() << "\nExiting..." << std::endl;
            }
            break;
        }
        auto mtype = static_cast<MessageType>(itype);
        assert(mtype == MessageType::BROADCAST);
        auto message = clientSocket->read_broadcast();
        if (message != last_message) {
            std::unique_lock<std::mutex> print_lock{ print_mutex };
            std::cout << message << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage %s <hostname> <port> <username>\n", argv[0]);
        exit(0);
    }

    const std::string hostname = argv[1];
    const std::string portstr = argv[2];
    const std::string username = argv[3];
    auto const portno = stoi(portstr);
    try {
        Socket::init();
        clientSocket = std::make_shared<Socket>(hostname, portno, username);
        std::thread broadcast_thread{ broadcast_callback };
        while (!is_finished) {
            std::cout << "Please enter the message: " << std::endl;
            std::string line;
            std::getline(std::cin, line);
            if (line.empty()) {
                clientSocket->write_uint(static_cast<int>(MessageType::FINISH));
#if _WIN32
                // Waiting for server to acknowledge the finish in order to close the socket here without exception there.
                Sleep(1000);
#endif
                is_finished = true;
                break;
            } else {
                auto date = Date::now();
                clientSocket->write_message(line, date);
                last_message = Message(line, clientSocket->this_username, date);
            }
        }
        broadcast_thread.detach();
    }
    catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    WSACleanup();
    return 0;
}

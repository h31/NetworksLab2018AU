#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cassert>

#include "DnsAll.h"

static SocketWrapper clientSocket;
volatile bool is_finished = false;

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
        bool broadcast_on = true;
        while (!is_finished) {
            std::cout << "Please enter the message: " << std::endl;
            std::string line;
            std::getline(std::cin, line);
            if (line.empty()) {
                clientSocket->write_uint(static_cast<int>(MessageType::FINISH));
                is_finished = true;
                break;
            } else {
                // TODO write dns request.
            }
        }
    }
    catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}

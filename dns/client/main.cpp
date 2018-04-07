#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cassert>

#include "DnsAll.h"

static DnsSocketWrapper clientSocket;
volatile bool is_finished = false;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage %s <hostname> <port>\n", argv[0]);
        return 1;
    }

    const std::string hostname = argv[1];
    const std::string portstr = argv[2];
    auto const portno = stoi(portstr);
    try {
        clientSocket.reset(new DnsSocket(hostname, portno));
        while (!is_finished) {
            std::cout << "Please enter address: " << std::endl;
            std::string line;
            std::getline(std::cin, line);
            if (line.empty()) {
                clientSocket.reset();
                is_finished = true;
                break;
            } else {
                std::cout << "Answer: " << clientSocket->resolve(line) << std::endl;
            }
        }
    }
    catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}

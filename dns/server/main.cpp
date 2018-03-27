#include "Server.h"

#include <iostream>

int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "usage: <port>" << std::endl;
        return 0;
    }

    auto port = static_cast<uint16_t>(atoi(argv[1]));

    Server server(port);
    server.start();

    std::cout << "type \"quit\" to stop server." << std::endl;

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (line == "quit") {
            server.stop();
            break;
        }
    }

    return 0;
}
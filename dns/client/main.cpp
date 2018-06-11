#include <iostream>
#include "Resolver.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "usage: ./client <dns server address> <dns server port>" << std::endl;
        return 0;
    }

    auto port = static_cast<uint16_t>(atoi(argv[2]));
    Resolver resolver(argv[1], port);
    resolver.start();

    std::cout << "type domain name to resolve it.\ntype \"quit\" to stop resolver." << std::endl;

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (line == "quit") {
            break;
        }

        std::string address = resolver.resolve(line);
        std::cout << address << std::endl;
    }

    return 0;
}
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include "DnsAll.h"

static volatile bool is_finished = false;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        return 1;
    }
    try {
        auto const port = stoi(static_cast<std::string>(argv[1]));
        DnsServerSocket serverSocket{port};
        while (!is_finished) {
            serverSocket.step();
        }
    }
    catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        is_finished = true;
    }
    return 0;
}

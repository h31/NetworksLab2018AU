#include <iostream>
#include "Server.h"

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cout << "expected 1 argument in format: <port>" << std::endl;
        exit(0);
    }

    auto port = (uint16_t) atoi(argv[1]);
    Server server(port);
    while (true) {
        int c = getchar();
        if (c == 'q') {
            break;
        }
    }
}
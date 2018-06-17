#include <iostream>
#include "server.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "usage: hosts_file port" << std::endl;
        return 0;
    }
    std::string hosts_file = argv[1];
    int port = atoi(argv[2]);
    server s{hosts_file, port};
    s.run();
    return 0;
}


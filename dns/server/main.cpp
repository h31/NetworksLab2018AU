#include <iostream>
#include "Server.h"

int main (int argc, char **argv)
{
    if (argc != 3) {
        std::cout << "usage: port path_to_hosts_file" << std::endl;
        return 0;
    }
    Server server;

    server.init(argv[2]);
    uint16_t port = atoi(argv[1]);
    server.open_sockets(port);
    server.run();

    return 0;
}
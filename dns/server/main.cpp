#include "Server.h"

int main (int argc, char **argv)
{
    Server server;

    server.init(argv[2]);
    uint16_t port = atoi(argv[1]);
    server.open_sockets(port);
    server.run();

    return 0;
}
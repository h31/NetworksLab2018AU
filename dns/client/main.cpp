#include "Client.h"

int main (int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "usage: hostname port" << std::endl;
        return 0;
    }
    auto port = (uint16_t) atoi(argv[2]);
    Client client(argv[1], port);
    client.run();

    return 0;
}
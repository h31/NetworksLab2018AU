#include "Client.h"

int main (int argc, char *argv[])
{
    auto port = (uint16_t) atoi(argv[2]);
    Client client(argv[1], port);
    client.run();

    return 0;
}
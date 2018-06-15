#include <iostream>
#include "dns_client.h"

int main (int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "usage: hostname port" << std::endl;
        return 0;
    }
    auto port = (uint16_t) atoi(argv[2]);
    dns_client client(argv[1], port);

    std::string line;
    while (true) {
        std::cout << "Enter a domain name or ':q'" << std::endl;
        std::cin >> line;
        if (line == ":q") {
            break;
        }
        client.gethost(line);
    }
}

#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <thread>
#include "Client.h"

int main(int argc, char *argv[]) {
    std::cout.setf(std::ios::unitbuf);
    
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    auto port = (uint16_t) atoi(argv[2]);
    Client client(argv[1], port, argv[3]);
    client.run();
    return 0;
}
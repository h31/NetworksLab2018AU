#include "ServerSocket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdexcept>
#include <cstring>
#include <iostream>

ServerSocket::ServerSocket(uint16_t port) : Socket() {
    if (socketDescriptor < 0) {
        throw std::runtime_error("ERROR opening socket");
    }

    std::cout << "starting server at socket " << socketDescriptor << std::endl;

    struct sockaddr_in serverAddress;
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        throw std::runtime_error("ERROR on binding");
    }
}

Socket ServerSocket::accept() {
    // todo what is this
    listen(socketDescriptor, 5);

    struct sockaddr_in clientAddress;
    unsigned int clilen = sizeof(clientAddress);
    int newSocketDescriptor = ::accept(socketDescriptor, (struct sockaddr *) &clientAddress, &clilen);

    if (newSocketDescriptor < 0) {
        throw std::runtime_error("ERROR on accept");
    }

    return Socket{newSocketDescriptor};
}

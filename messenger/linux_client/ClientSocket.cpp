#include "ClientSocket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

#include <vector>
#include <iostream>

ClientSocket::ClientSocket(const std::string &host, uint16_t port) : Socket() {
    if (socketDescriptor < 0) {
        throw std::runtime_error("ERROR opening socket");
    }

    std::cout << "starting client at socket " << socketDescriptor << std::endl;

    // todo fix memory leak
    struct hostent *server = gethostbyname(host.c_str());
    if (server == nullptr) {
        throw std::runtime_error("ERROR, no such host\n");
    }

    struct sockaddr_in serverAddress;
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serverAddress.sin_addr.s_addr, (size_t) server->h_length);
    serverAddress.sin_port = htons(port);

    if (connect(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        throw std::runtime_error("ERROR connecting");
    }
}

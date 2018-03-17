#include <iostream>
#include "Server.h"

Server::Server(uint16_t port) :
        serverSocket(port),
        server(&Server::serverRoutine, this)
{}

Server::~Server() {
    std::cout << "server closed!" << std::endl;
}

void Server::serverRoutine() {
    std::cout << "server started!" << std::endl;
    while (true) {
        Socket client = serverSocket.accept();
        std::cout << "client accepted!" << std::endl;
    }
}

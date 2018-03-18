#include "ServerSocket.h"

#include <stdexcept>
#include <cstring>
#include <iostream>

ServerSocket::ServerSocket(uint16_t port) : Socket() {
    if (socketDescriptor < 0) {
        throw std::runtime_error("ERROR opening socket");
    }

    std::cout << "starting server at socket " << socketDescriptor << std::endl;
#ifndef _WIN32
    struct sockaddr_in serverAddress;
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        throw std::runtime_error("ERROR on binding");
    }

    // todo what is this
    listen(socketDescriptor, 5);

#else
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	std::string strP = std::to_string(port);
	auto iResult = getaddrinfo(NULL, strP.c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		throw std::runtime_error("getaddrinfo failed");
	}

	iResult = bind(socketDescriptor, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(socketDescriptor);
		WSACleanup();
		throw std::runtime_error("bind failed with error");
	}

	freeaddrinfo(result);

	if (listen(socketDescriptor, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(socketDescriptor);
		WSACleanup();
		throw std::runtime_error("Listen failed with error");
	}
#endif
}

Socket ServerSocket::accept() {
    struct sockaddr_in clientAddress;
    socklen_t clilen = sizeof(clientAddress);
    int newSocketDescriptor = ::accept(socketDescriptor, (struct sockaddr *) &clientAddress, &clilen);

    if (newSocketDescriptor < 0) {
        throw std::runtime_error("ERROR on accept");
    }

    return Socket{newSocketDescriptor};
}

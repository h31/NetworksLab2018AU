#include "ServerSocket.h"

#include <stdexcept>
#include <cstring>
#include <iostream>

ServerSocket::ServerSocket(uint16_t port) : Socket(0) {

#ifndef _WIN32
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
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

    listen(socketDescriptor, 5);

#else

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		throw std::runtime_error("WSAStartup failed with error");
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	std::string portS = std::to_string(port);
	iResult = getaddrinfo(NULL, portS.c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		throw std::runtime_error("getaddrinfo failed with error");
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		throw std::runtime_error("socket failed with error");
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		throw std::runtime_error("bind failed with error");
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(ListenSocket);
		WSACleanup();
		throw std::runtime_error("listen failed with error");
	}

	socketDescriptor = ListenSocket;
#endif
}

Socket ServerSocket::accept() {
    struct sockaddr_in clientAddress;
    socklen_t socklen = sizeof(clientAddress);
    int newSocketDescriptor = ::accept(socketDescriptor, (struct sockaddr *) &clientAddress, &socklen);

    if (newSocketDescriptor < 0) {
        throw std::runtime_error("ERROR on accept");
    }

    return Socket{newSocketDescriptor};
}

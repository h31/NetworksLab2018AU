#include "ClientSocket.h"

#include <cstring>
#include <stdexcept>

#include <vector>
#include <iostream>

ClientSocket::ClientSocket(const std::string &host, uint16_t port) : Socket(0) {
#ifndef _WIN32
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (socketDescriptor < 0) {
		throw std::runtime_error("ERROR opening socket");
	}

	std::cout << "starting client at socket " << socketDescriptor << std::endl;
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
#else
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		throw std::runtime_error("failed to startup");
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	std::string portS = std::to_string(port);
	iResult = getaddrinfo(host.c_str(), portS.c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		throw std::runtime_error("getaddrinfo failed with error");
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			WSACleanup();
			throw std::runtime_error("socket failed with error");
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		throw std::runtime_error("Unable to connect to server!");
	}

	socketDescriptor = ConnectSocket;
#endif
}

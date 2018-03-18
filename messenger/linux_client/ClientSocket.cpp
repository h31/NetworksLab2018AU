#include "ClientSocket.h"

#include <cstring>
#include <stdexcept>

#include <vector>
#include <iostream>

ClientSocket::ClientSocket(const std::string &host, uint16_t port) : Socket() {
    if (socketDescriptor < 0) {
        throw std::runtime_error("ERROR opening socket");
    }

    std::cout << "starting client at socket " << socketDescriptor << std::endl;

#ifndef _WIN32
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
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	std::string strP = std::to_string(port);
	auto iResult = getaddrinfo(host.c_str(), strP.c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		throw std::runtime_error("getaddrinfo failed");
	}


#endif
}

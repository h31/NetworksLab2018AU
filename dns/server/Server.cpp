#include "Server.h"
#include "dnsUtils.h"

#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <unistd.h>

Server::Server(uint16_t serverPort) : serverPort(serverPort), socketDescriptor(-1), stopped(false) {}

void Server::start() {
    initSocket();
    std::cout << "server started" << std::endl;
    worker = std::thread([this]() {
        while (!stopped) {
            serveClient();
        }
    });
}

void Server::initSocket() {
    if ((socketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        throw std::runtime_error("failed to get socket");
    }

    sockaddr_in serverAddress{};
    memset((char *) &serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    timeval tv{};
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof tv);

    if (bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        throw std::runtime_error("failed to bind");
    }
}

std::string resolve(const std::string &hostName) {
    hostent *record = gethostbyname(hostName.c_str());
    if (!record) {
        return "0.0.0.0";
    }
    auto *address = (in_addr *) record->h_addr;
    std::string ip_address = inet_ntoa(*address);
    return ip_address;
}

void Server::serveClient() {
    static const int BUFLEN = 60000;
    static std::vector<char> buf(BUFLEN);

    std::fill(buf.begin(), buf.end(), 0);

    sockaddr_in clientAddress{};
    socklen_t slen = sizeof(clientAddress);

    if (recvfrom(socketDescriptor, buf.data(), BUFLEN, 0, (sockaddr *) &clientAddress, &slen) < 0) {
        return;
    }

    std::cout << "got datagram" << std::endl;
    std::string address = parseHostNameFromDNSRequest(buf);
    std::cout << "requested address: " << address << std::endl;
    std::string result = resolve(address);
    size_t resLen = writeDNSAResponse(buf, address, result);

    if (sendto(socketDescriptor, buf.data(), resLen, 0, (sockaddr *) &clientAddress, slen) == -1) {
        throw std::runtime_error("sendto() failed");
    }
}

Server::~Server() {
    if (socketDescriptor != -1) {
        close(socketDescriptor);
        socketDescriptor = -1;
    }
}

void Server::stop() {
    stopped = true;
    if (worker.joinable()) {
        worker.join();
    }
}

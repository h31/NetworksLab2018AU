#include <iostream>
#include "Resolver.h"
#include "dnsUtils.h"

#include<cstring>
#include<arpa/inet.h>
#include <unistd.h>

Resolver::Resolver(const std::string &serverAddress, uint16_t serverPort) :
        serverAddress(serverAddress),
        serverPort(serverPort),
        socketDescriptor(-1) {}

void Resolver::start() {
    if ((socketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        throw std::runtime_error("failed to get socket");
    }

    memset((char *) &socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(serverPort);

    if (inet_aton(serverAddress.c_str(), &socketAddress.sin_addr) == 0) {
        throw std::runtime_error("inet_aton() failed");
    }

    std::cout << "resolver started" << std::endl;
}

std::string Resolver::resolve(const std::string &domainName) {
    static const int BUFLEN = 60000;
    static std::vector<char> buf(BUFLEN);

    std::fill(buf.begin(), buf.end(), 0);
    size_t msgLen = writeDNSARequest(buf, domainName);

    socklen_t slen = sizeof(socketAddress);

    if (sendto(socketDescriptor, buf.data(), msgLen, 0, (sockaddr *) &socketAddress, slen) == -1) {
        throw std::runtime_error("sendto() failed");
    }

    if (recvfrom(socketDescriptor, buf.data(), BUFLEN, 0, (sockaddr *) &socketAddress, &slen) == -1) {
        throw std::runtime_error("recvfrom() failed");
    }

    std::string result = parseIPFromDNSResponse(buf);
    return result;
}

Resolver::~Resolver() {
    if (socketDescriptor != -1) {
        close(socketDescriptor);
        socketDescriptor = -1;
    }
}

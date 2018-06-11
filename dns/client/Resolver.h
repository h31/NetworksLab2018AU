#pragma once

#include <string>
#include <netinet/in.h>


struct Resolver {
    explicit Resolver(const std::string &serverAddress, uint16_t serverPort);

    ~Resolver();

    void start();

    std::string resolve(const std::string &domainName);

private:
    const std::string serverAddress;
    const uint16_t serverPort;
    int socketDescriptor;
    sockaddr_in socketAddress;
};



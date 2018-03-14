#include <iostream>
#include <limits>
#include <cstring>
#include "Socket.h"
#include "ElegramAll.h"

#define MESSAGE_LENGTH 256

Socket::Socket(int fd, sockaddr cli_addr, unsigned int clilen)
        : fd(fd)
        , cli_addr(cli_addr)
        , clilen(clilen)
{}

MessageWrapper Socket::read_message() {
    auto message = std::make_shared<Message>(MESSAGE_LENGTH);
    std::uint32_t n32 = 0;
    ssize_t nbytes = 0;
    auto check_reading = [&]() {
        if (nbytes < 0) {
            throw MessengerError("ERROR reading from socket");
        }
    };
    
    while (nbytes < (int)sizeof(n32)) {
        nbytes = read(fd, reinterpret_cast<char *>(&n32) + nbytes, sizeof(n32) - nbytes); // recv on Windows
        check_reading();
    }
    
    size_t n = ntohl(n32);
    message->buffer.resize(n);
    auto message_ptr = &message->buffer[0];
    nbytes = 0;
    while (nbytes < (ssize_t)n) {
        nbytes = read(fd, message_ptr + nbytes, n - nbytes); // recv on Windows
        check_reading();
    }
    return message;
}

void Socket::write_message(MessageWrapper message) {
    auto size = message->size();
    if (size < 0 || size > std::numeric_limits<std::uint32_t>::max()) {
        throw MessengerError("Invalid message size : " + std::to_string(size));
    }
    
    ssize_t nbytes = 0;
    auto check_reading = [&]() {
        if (nbytes < 0) {
            throw MessengerError("ERROR writing to socket");
        }
    };
    
    std::uint32_t n32 = htonl(size);
    while (nbytes < sizeof(n32)) {
        nbytes = ::write(fd, reinterpret_cast<char *>(&n32) + nbytes, sizeof(n32) - nbytes);
        check_reading();
    }
    
    nbytes = 0;
    while (nbytes < size) {
        nbytes = ::write(fd, message->ptr(), size - static_cast<size_t>(nbytes)); // send on Windows
        check_reading();
    }
}

Socket::Socket(const std::string &hostname, int portno) {
    struct hostent *server;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (fd < 0) {
        throw MessengerError("ERROR opening socket");
    }
    
    server = gethostbyname(hostname.c_str());
    if (server == nullptr) {
        throw MessengerError("ERROR, no such host");
    }
    
    struct sockaddr_in connect_addr;
    bzero((char *) &connect_addr, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &connect_addr.sin_addr.s_addr, (size_t) server->h_length);
    connect_addr.sin_port = htons(portno);
    
    if (connect(fd, (struct sockaddr *) &connect_addr, sizeof(connect_addr)) < 0) {
        throw MessengerError("ERROR connecting");
    }
    
    clilen = sizeof(cli_addr);
    bcopy(&connect_addr, &cli_addr, clilen);
}

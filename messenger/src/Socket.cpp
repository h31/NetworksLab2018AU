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
    auto message_ptr = &message->buffer[0];
    bzero(message_ptr, MESSAGE_LENGTH);
    ssize_t n = read(fd, message_ptr, MESSAGE_LENGTH - 1); // recv on Windows
    if (n < 0) {
        throw MessengerError("ERROR reading from socket");
    }
    message->buffer.resize(n);
    return message;
}

void Socket::write_message(MessageWrapper message) {
    auto n = message->size();
    if (n < 0 || n > std::numeric_limits<std::uint32_t>::max()) {
        throw MessengerError("Invalid message size : " + std::to_string(n));
    }
    n = ::write(fd, message->ptr(), n); // send on Windows
    if (n < 0) {
        throw MessengerError("ERROR writing to socket");
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

#include "Socket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

#include <vector>
#include <iostream>
#include <csignal>


const uint32_t MAGIC = 0xDEADBEEF;

Socket::Socket() : socketDescriptor(socket(AF_INET, SOCK_STREAM, 0)) {}

Socket::Socket(int socketDescriptor) : socketDescriptor(socketDescriptor) {}

Socket::Socket(Socket &&other) noexcept : socketDescriptor(other.socketDescriptor) {
    other.socketDescriptor = -1;
}

Socket::~Socket() {
    close();
}

void readString(std::vector<char> & buffer, int fd) {
    size_t length;
    ssize_t r = 0;
    
    r = read(fd, &length, sizeof(length));
    if (r == 0) throw SocketException();
    
    if (buffer.size() < length + 1) {
        buffer.resize(length + 1);
    }
    r = read(fd, buffer.data(), length + 1);
    if (r == 0) throw SocketException();
}

void writeString(const std::string & str, int fd) {
    size_t length = str.size();
    write(fd, &length, sizeof(length));
    write(fd, str.c_str(), length + 1);
}

Message Socket::read() {

    std::vector<char> buffer(256);

    uint32_t magic;
    ssize_t r = 0;
    r = ::read(socketDescriptor, &magic, sizeof(magic));
    if (r == 0 || magic != MAGIC) {
        throw SocketException();
    }

    readString(buffer, socketDescriptor);
    std::string text(buffer.data());

    readString(buffer, socketDescriptor);
    std::string time(buffer.data());

    readString(buffer, socketDescriptor);
    std::string nickname(buffer.data());

    return Message{text, time, nickname};
}

void brokenPipeHandler(int signal) {
    std::cout << "broken pipe ignored" << std::endl;
}

void Socket::write(const Message &message) {
    signal(SIGPIPE, brokenPipeHandler);
    ::write(socketDescriptor, &MAGIC, sizeof(MAGIC));
    writeString(message.text, socketDescriptor);
    writeString(message.time, socketDescriptor);
    writeString(message.nickname, socketDescriptor);
}

void Socket::close() {
    if (socketDescriptor != -1) {
        ::close(socketDescriptor);
        socketDescriptor = -1;
    }
}

bool Socket::interesting() {
    fd_set iset;
    FD_ZERO(&iset);
    FD_SET(socketDescriptor, &iset);

    struct timeval tv;
    tv.tv_sec = (long) 1;
    tv.tv_usec = 0;

    int interestSetSize = select(socketDescriptor + 1, &iset, nullptr, nullptr, &tv);
    return interestSetSize > 0;
}

int Socket::descriptor() const {
    return socketDescriptor;
}

const char *SocketException::what() const noexcept {
    return "could not parse message: did not receive magic number";
}

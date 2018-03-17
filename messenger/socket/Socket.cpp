#include "Socket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

#include <vector>
#include <iostream>

Socket::Socket() : socketDescriptor(socket(AF_INET, SOCK_STREAM, 0)) {}

Socket::Socket(int socketDescriptor) : socketDescriptor(socketDescriptor) {}

void readString(std::vector<char> & buffer, int fd) {
    size_t length;
    read(fd, &length, sizeof(length));
    if (buffer.size() < length + 1) {
        buffer.resize(length + 1);
    }
    read(fd, buffer.data(), length + 1);
}

void writeString(const std::string & str, int fd) {
    size_t length = str.size();
    write(fd, &length, sizeof(length));
    write(fd, str.c_str(), length + 1);
}

Message Socket::read() {

    std::vector<char> buffer(256);

    readString(buffer, socketDescriptor);
    std::string text(buffer.data());

    readString(buffer, socketDescriptor);
    std::string time(buffer.data());

    readString(buffer, socketDescriptor);
    std::string nickname(buffer.data());

    return Message{text, time, nickname};
}

void Socket::write(const Message &message) {
    writeString(message.text, socketDescriptor);
    writeString(message.time, socketDescriptor);
    writeString(message.nickname, socketDescriptor);
}

Socket::~Socket() {
    close(socketDescriptor);
    std::cout << "socket closed!" << std::endl;
}

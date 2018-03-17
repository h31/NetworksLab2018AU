#include "Socket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <stdexcept>

#include <vector>

Socket::Socket(const std::string &host, uint16_t port) : host(host), port(port), socketDescriptor(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socketDescriptor < 0) {
        throw std::runtime_error("ERROR opening socket");
    }

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
}

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
}

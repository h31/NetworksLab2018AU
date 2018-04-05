#include <iostream>
#include <limits>
#include <cstring>
#include "Socket.h"
#include "DnsAll.h"

Socket::Socket(socket_t fd, sockaddr cli_addr, const std::string &username)
        : fd(fd)
        , cli_addr(cli_addr)
        , this_username(username)
{
    std::cout << "Creating socket on server: " << fd << std::endl;
    auto message_type = static_cast<MessageType>(read_uint());
    if (message_type != MessageType::INIT_FROM_CLIENT) {
        throw ProtocolError("Error with init while creating socket on server.");
    }
    other_username = read_string();
    write_uint(static_cast<uint32_t>(MessageType::INIT_FROM_SERVER));
    write_string(this_username);
}

Socket::Socket(const std::string &hostname, int portno, const std::string &username, int socket_type)
        : this_username(username)
{
    auto protocol = 0;
    fd = socket(AF_INET, socket_type, protocol);

    if (fd < 0) {
        throw DnsError("ERROR opening socket");
    }
    struct hostent *server = gethostbyname(hostname.c_str());
    if (server == nullptr) {
        throw DnsError("ERROR, no such host");
    }
    
    struct sockaddr_in connect_addr{};
    memset((char *)&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    memcpy((char *) &connect_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    connect_addr.sin_port = htons(portno);
    
    auto const connect_result = connect(fd, (struct sockaddr *) &connect_addr, sizeof(connect_addr));
    if (connect_result < 0) {
        throw DnsError("ERROR connecting");
    }
    
    memcpy(&cli_addr, &connect_addr, sizeof(cli_addr));

    write_uint(static_cast<uint32_t>(MessageType::INIT_FROM_CLIENT));
    write_string(username);
    auto message_type = static_cast<MessageType>(read_uint());
    if (message_type != MessageType::INIT_FROM_SERVER) {
        throw ProtocolError("Error with init while creating socket on client.");
    }
    other_username = read_string();
}

std::string Socket::read_string() const {
    size_t n = read_uint();
    std::string buffer;
    buffer.resize(n);
    auto message_ptr = &buffer[0];
    ssize_t nbytes = 0;
    while (nbytes < (ssize_t)n) {
        nbytes = read(fd, message_ptr + nbytes, n - static_cast<size_t>(nbytes)); // recv on Windows
        check_io(nbytes, "reading");
    }
    return buffer;
}

void Socket::write_string(const std::string &str) const {
    auto size = str.size();
    if (size > std::numeric_limits<std::uint32_t>::max()) {
        throw DnsError("Invalid message size : " + std::to_string(size));
    }
    
    write_uint(static_cast<uint32_t>(size));
    ssize_t nbytes = 0;
    while (nbytes < static_cast<ssize_t>(size)) {
        nbytes = write(fd, &str[0], size - static_cast<size_t>(nbytes)); // send on Windows
        check_io(nbytes, "writing");
    }
}

Socket::~Socket() {
    close(fd);
    shutdown(fd, SHUT_RDWR);
}

void Socket::init() {
}

void Socket::check_io(ssize_t nbytes, const std::string &process) {
    if (nbytes < 0) {
        throw DnsError("Error " + process + " socket");
    }
}

ssize_t Socket::read(socket_t fd, char * buf, size_t size) {
    return ::read(fd, buf, size);
}

ssize_t Socket::write(socket_t fd, const char * buf, size_t size) {
    return ::write(fd, buf, size);
}

std::uint32_t Socket::read_uint() const {
    ssize_t nbytes = 0;
    std::uint32_t n32;
    while (nbytes < (int)sizeof(n32)) {
        nbytes = read(fd, reinterpret_cast<char *>(&n32) + nbytes, sizeof(n32) - static_cast<size_t>(nbytes));
        check_io(nbytes, "reading");
    }
    auto result = ntohl(n32);
    return result;
}

void Socket::write_uint(std::uint32_t n) const {
    ssize_t nbytes = 0;
    std::uint32_t n32 = htonl(n);
    while (nbytes < (ssize_t)sizeof(n32)) {
        nbytes = write(fd, reinterpret_cast<char *>(&n32) + nbytes, sizeof(n32) - static_cast<size_t>(nbytes));
        check_io(nbytes, "writing");
    }
}

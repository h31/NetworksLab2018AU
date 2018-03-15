#include <iostream>
#include <limits>
#include <cstring>
#include "Socket.h"
#include "ElegramAll.h"

Socket::Socket(int fd, sockaddr cli_addr, unsigned int clilen, const std::string &username)
        : fd(fd)
        , cli_addr(cli_addr)
        , clilen(clilen)
        , this_username(username)
{
    std::cout << "Creating socket on server: " << fd << std::endl;

#if DEBUG_PROTOCOL
    std::cout << "Initializing new outcoming connection..." << std::endl;
#endif
    auto message_type = static_cast<MessageType>(read_uint());
    if (message_type != MessageType::INIT_FROM_CLIENT) {
        throw ProtocolError("Error with init while creating socket on server.");
    }
    other_username = read_string();
    write_uint(static_cast<uint32_t>(MessageType::INIT_FROM_SERVER));
    write_string(this_username);
#if DEBUG_PROTOCOL
    std::cout << "...finished: other name: " << other_username << std::endl;
#endif
}

Socket::Socket(const std::string &hostname, int portno, const std::string &username)
        : this_username(username)
{
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

#if DEBUG_PROTOCOL
    std::cout << "Initializing new incoming connection..." << std::endl;
#endif
    write_uint(static_cast<uint32_t>(MessageType::INIT_FROM_CLIENT));
    write_string(username);
    auto message_type = static_cast<MessageType>(read_uint());
    if (message_type != MessageType::INIT_FROM_SERVER) {
        throw ProtocolError("Error with init while creating socket on client.");
    }
    other_username = read_string();
#if DEBUG_PROTOCOL
    std::cout << "finished. Other name: " << other_username << std::endl;
#endif
}

std::string Socket::read_string() {
#if DEBUG_PROTOCOL
    std::cout << "Reading string" << std::endl;
#endif
    size_t n = read_uint();
    std::string buffer;
    buffer.resize(n);
    auto message_ptr = &buffer[0];
    ssize_t nbytes = 0;
    while (nbytes < (ssize_t)n) {
        nbytes = read(fd, message_ptr + nbytes, n - nbytes); // recv on Windows
        check_reading(nbytes);
    }
#if DEBUG_PROTOCOL
    std::cout << "Read string: " << buffer << std::endl;
#endif
    return buffer;
}

void Socket::write_string(const std::string &str) {
#if DEBUG_PROTOCOL
    std::cout << "Writing string: " << str << std::endl;
#endif
    auto size = str.size();
    if (size > std::numeric_limits<std::uint32_t>::max()) {
        throw MessengerError("Invalid message size : " + std::to_string(size));
    }
    
    write_uint(size);
    ssize_t nbytes = 0;
    while (nbytes < static_cast<ssize_t>(size)) {
        nbytes = ::write(fd, &str[0], size - static_cast<size_t>(nbytes)); // send on Windows
        check_reading(nbytes);
    }
}

Socket::~Socket() {
    close(fd);
}

void Socket::check_reading(ssize_t nbytes) {
    if (nbytes < 0) {
        throw MessengerError("ERROR writing to socket");
    }
}

std::uint32_t Socket::read_uint() {
#if DEBUG_PROTOCOL
    std::cout << "Reading uint" << std::endl;
#endif
    ssize_t nbytes = 0;
    std::uint32_t n32;
    while (nbytes < (int)sizeof(n32)) {
        nbytes = read(fd, reinterpret_cast<char *>(&n32) + nbytes, sizeof(n32) - nbytes); // recv on Windows
        check_reading(nbytes);
    }
    auto result = ntohl(n32);
#if DEBUG_PROTOCOL
    std::cout << "Read uint: " << result << std::endl;
#endif
    return result;
}

void Socket::write_uint(std::uint32_t n) {
#if DEBUG_PROTOCOL
    std::cout << "Writing uint: " << n << std::endl;
#endif
    ssize_t nbytes = 0;
    std::uint32_t n32 = htonl(n);
    while (nbytes < (ssize_t)sizeof(n32)) {
        nbytes = ::write(fd, reinterpret_cast<char *>(&n32) + nbytes, sizeof(n32) - nbytes);
        check_reading(nbytes);
    }
}

MessageWrapper Socket::read_message() {
#if DEBUG_PROTOCOL
    std::cout << "Reading message" << std::endl;
#endif
    auto buffer = read_string();
    auto date_string = read_string();
    auto date = Date::from_string(date_string);
    auto message = std::make_shared<Message>(buffer, other_username, date);
#if DEBUG_PROTOCOL
    std::cout << "Read message: " << message->to_string() << std::endl;
#endif
    return message;
}

void Socket::write_message(const std::string &buffer, const Date &date) {
#if DEBUG_PROTOCOL
    std::cout << "Writing message: " << date.to_string() << ": " << buffer << std::endl;
#endif
    write_uint(static_cast<uint32_t>(MessageType::MESSAGE));
    write_string(buffer);
#if DEBUG_PROTOCOL
    std::cout << "Writing date: " << date.to_string() << std::endl;
#endif
    write_string(date.to_string());
}

void Socket::finish() {
    write_uint(static_cast<std::uint32_t>(MessageType::FINISH));
}

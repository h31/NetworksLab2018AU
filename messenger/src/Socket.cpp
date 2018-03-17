#include <iostream>
#include <limits>
#include <cstring>
#include "Socket.h"
#include "ElegramAll.h"

#if _WIN32
static u_long zero = 0;
#endif

Socket::Socket(socket_t fd, sockaddr cli_addr, const std::string &username)
        : fd(fd)
        , cli_addr(cli_addr)
        , this_username(username)
{
    std::cout << "Creating socket on server: " << fd << std::endl;
#if _WIN32
    auto ioctl_error = ioctlsocket(fd, FIONBIO, &zero);
    if (ioctl_error == SOCKET_ERROR) {
        throw MessengerError("Failed to turn on blocking mode for socket: " + std::to_string(WSAGetLastError()));
    }
#endif
    auto message_type = static_cast<MessageType>(read_uint());
    if (message_type != MessageType::INIT_FROM_CLIENT) {
        throw ProtocolError("Error with init while creating socket on server.");
    }
    other_username = read_string();
    write_uint(static_cast<uint32_t>(MessageType::INIT_FROM_SERVER));
    write_string(this_username);
}

Socket::Socket(const std::string &hostname, int portno, const std::string &username)
        : this_username(username)
{
    auto protocol = 0;
#if _WIN32
    protocol = IPPROTO_TCP;
#endif
    fd = socket(AF_INET, SOCK_STREAM, protocol);

#if _WIN32
    auto ioctl_error = ioctlsocket(fd, FIONBIO, &zero);
    if (ioctl_error == SOCKET_ERROR) {
        throw MessengerError("Failed to turn on blocking mode for socket: " + std::to_string(WSAGetLastError()));
    }
    if (fd == INVALID_SOCKET) {
#else
    if (fd < 0) {
#endif
        throw MessengerError("ERROR opening socket");
    }
    struct hostent *server = gethostbyname(hostname.c_str());
    if (server == nullptr) {
        throw MessengerError("ERROR, no such host");
    }
    
    struct sockaddr_in connect_addr;
    memset((char *)&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    memcpy((char *) &connect_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    connect_addr.sin_port = htons(portno);
    
    auto const connect_result = connect(fd, (struct sockaddr *) &connect_addr, sizeof(connect_addr));
#if _WIN32
    if (connect_result == SOCKET_ERROR) {
#else
    if (connect_result < 0) {
#endif
        throw MessengerError("ERROR connecting");
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
        throw MessengerError("Invalid message size : " + std::to_string(size));
    }
    
    write_uint(size);
    ssize_t nbytes = 0;
    while (nbytes < static_cast<ssize_t>(size)) {
        nbytes = write(fd, &str[0], size - static_cast<size_t>(nbytes)); // send on Windows
        check_io(nbytes, "writing");
    }
}

Socket::~Socket() {
#if _WIN32
    closesocket(fd);
#else
    close(fd);
#endif
}

void Socket::init() {
#if _WIN32
    WSADATA wsaData;
    auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        throw MessengerError("WSAStartup failed with error: " + std::to_string(iResult));
    }
#endif
}

void Socket::check_io(ssize_t nbytes, const std::string &process) {
    if (nbytes < 0) {
        throw MessengerError("Error " + process + " socket");
    }
}

ssize_t Socket::read(socket_t fd, char * buf, size_t size)
{
#if _WIN32
    return recv(fd, buf, size, 0);
#else
    return ::read(fd, buf, size);
#endif
}

ssize_t Socket::write(socket_t fd, const char * buf, size_t size)
{
#if _WIN32
    return send(fd, buf, size, 0);
#else
    return ::write(fd, buf, size);
#endif
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

Message Socket::read_message() const {
    auto buffer = read_string();
    auto date_string = read_string();
    auto date = Date::from_string(date_string);
    Message message{buffer, other_username, date};
    return message;
}

void Socket::write_message(const std::string &buffer, const Date &date) const {
    write_uint(static_cast<uint32_t>(MessageType::MESSAGE));
    write_string(buffer);
    write_string(date.to_string());
}

Message Socket::read_broadcast() const {
    auto username = read_string();
    auto buffer = read_string();
    auto date_string = read_string();
    auto date = Date::from_string(date_string);
    Message message{buffer, username, date};
    return message;
}

void Socket::write_broadcast(const Message &message) const {
    write_uint(static_cast<uint32_t>(MessageType::BROADCAST));
    write_string(message.get_username());
    write_string(message.get_buffer());
    write_string(message.get_date().to_string());
}

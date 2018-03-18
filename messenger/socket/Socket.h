#pragma once

#ifndef _WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKET_READ(x, y, z) ::read(x, y, z)
#define SOCKET_WRITE(x, y, z) ::write(x, (char *) y, z, 0)
#define SOCKET_CLOSE(x) ::close(x)
#define SOCKET_SIGPIPE SIGPIPE 
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#define SOCKET_READ(x, y, z) recv(x, (char *) y, z, 0)
#define SOCKET_WRITE(x, y, z) send(x, (char *) y, z, 0)
#define SOCKET_CLOSE(x) closesocket(x)
#define SOCKET_SIGPIPE WSAECONNRESET 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <string>
#include "Message.h"

struct SocketException : std::exception {
    const char *what() const noexcept override;
};

struct Socket {
    Socket();
    explicit Socket(int socketDescriptor);
    Socket(Socket && other) noexcept;
    Socket(const Socket & other) = default;
    virtual ~Socket();
    Message read();
    void write(const Message & message);
    bool interesting();
    void close();
    int descriptor() const;

protected:
    int socketDescriptor;
    bool dead;
};



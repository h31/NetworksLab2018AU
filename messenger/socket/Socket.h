#pragma once

#ifndef _WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKET_READ(x, y, z) ::read((x), (char *) (y), (z))
#define SOCKET_WRITE(x, y, z) ::write((x), (char *) (y), (z))
#define SOCKET_CLOSE(x) ::close(x)
#define SOCKET_SIGPIPE SIGPIPE 
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define SOCKET_READ(x, y, z) recv((x), (char *) (y), (z), 0)
#define SOCKET_WRITE(x, y, z) send((x), (char *) (y), (z), 0)
#define SOCKET_CLOSE(x) ::closesocket(x)
#define SOCKET_SIGPIPE WSAECONNRESET
#endif

#include <string>
#include "Message.h"

struct SocketException : std::exception {
    const char *what() const noexcept override;
};

struct Socket {
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



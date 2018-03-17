#include <cstring>
#include "ServerSocket.h"
#include "ElegramAll.h"


ServerSocket::ServerSocket(int portno) {
    auto protocol = 0;
#if _WIN32
    protocol = IPPROTO_TCP;
#endif
    fd = socket(AF_INET, SOCK_STREAM, protocol);

    static u_long zero = 0;
    auto ioctl_error = ioctlsocket(fd, FIONBIO, &zero);
    if (ioctl_error == SOCKET_ERROR) {
        throw MessengerError("Failed to turn on blocking mode for socket: " + std::to_string(WSAGetLastError()));
    }

#if _WIN32
    if (fd == INVALID_SOCKET) {
#else
    if (sockfd < 0) {
#endif
        throw MessengerError("ERROR opening socket");
    }
    
    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    auto bind_result = bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
#if _WIN32
    if (bind_result == SOCKET_ERROR) {
        auto const error_msg = std::to_string(WSAGetLastError());
#else
    if (bind_result < 0) {
        auto const error_msg = std::to_string(bind_result);
#endif
        throw MessengerError("ERROR on binding: " + error_msg);
    }
}

void ServerSocket::listen(int nrequests) {
    ::listen(fd, nrequests);
}

SocketWrapper ServerSocket::accept() {
    struct sockaddr cli_addr = {};
    int clilen = sizeof(cli_addr);
    /* Accept actual connection from the client */
    auto cli_fd = ::accept(fd, &cli_addr, &clilen);
    
#if _WIN32
    if (cli_fd == SOCKET_ERROR) {
#else
    if (cli_fd < 0) {
#endif
        throw MessengerError("ERROR on accept: " + std::to_string(fd));
    }
    return std::make_shared<Socket>(cli_fd, cli_addr, "SERVER");
}

ServerSocket::~ServerSocket() {
    if (fd != 0) {
#if _WIN32
        closesocket(fd);
        //shutdown(fd, SD_SEND);
#else
        close(fd);
#endif
    }
}

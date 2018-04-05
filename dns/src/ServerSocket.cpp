#include <cstring>
#include "ServerSocket.h"
#include "DnsAll.h"


ServerSocket::ServerSocket(int portno) {
    auto protocol = 0;
    fd = socket(AF_INET, SOCK_STREAM, protocol);

    if (fd < 0) {
        throw DnsError("ERROR opening socket");
    }
    
    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    auto bind_result = bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (bind_result < 0) {
        auto const error_msg = std::to_string(bind_result);
        throw DnsError("ERROR on binding: " + error_msg);
    }
}

void ServerSocket::listen(int nrequests) {
    ::listen(fd, nrequests);
}

SocketWrapper ServerSocket::accept() {
    struct sockaddr cli_addr = {};
    unsigned int clilen = sizeof(cli_addr);
    /* Accept actual connection from the client */
    auto cli_fd = ::accept(fd, &cli_addr, &clilen);
    
    if (cli_fd < 0) {
        throw DnsError("ERROR on accept: " + std::to_string(fd));
    }
    return std::make_shared<Socket>(cli_fd, cli_addr, "SERVER");
}

ServerSocket::~ServerSocket() {
    if (fd != 0) {
        close(fd);
    }
}

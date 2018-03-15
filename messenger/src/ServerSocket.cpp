#include <cstring>
#include "ServerSocket.h"
#include "ElegramAll.h"

ServerSocket::ServerSocket(int portno) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
        throw MessengerError("ERROR opening socket");
    }
    
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    auto bind_result = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (bind_result < 0) {
        throw MessengerError("ERROR on binding: " + std::to_string(bind_result));
    }
    fd = sockfd;
}

void ServerSocket::listen(int nrequests) {
    ::listen(fd, nrequests);
}

Socket *ServerSocket::accept() {
    struct sockaddr cli_addr = {};
    unsigned int clilen = sizeof(cli_addr);
    
    /* Accept actual connection from the client */
    int cli_fd = ::accept(fd, &cli_addr, &clilen);
    
    if (fd < 0) {
        throw MessengerError("ERROR on accept: " + std::to_string(fd));
    }
    auto socket = new Socket(cli_fd, cli_addr, clilen, "SERVER");
    return socket;
}

ServerSocket::~ServerSocket() {
    if (fd != 0) {
        close(fd);
    }
}

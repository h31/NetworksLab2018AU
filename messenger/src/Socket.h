#ifndef MESSENGER_SOCKET_H
#define MESSENGER_SOCKET_H

#include <sys/socket.h>
#include "ElegramFwd.h"

struct Socket {
    Socket(int fd, sockaddr cli_addr, unsigned int clilen);
    
    Socket(const std::string &hostname, int port);
    
    MessageWrapper read_message();
    
    void write_message(MessageWrapper message);
    
    ~Socket() {}
    
    int fd;
    sockaddr cli_addr;
    unsigned int clilen;
};

#endif //MESSENGER_SOCKET_H

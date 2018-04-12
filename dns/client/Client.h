#ifndef DNS_CLIENT_H
#define DNS_CLIENT_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../Message.h"


class Client {
public:
    Client(char* hostname, uint16_t port);
    void run();
private:
    static const int BUFFER_SIZE = 256;
    int socket_fd;
    sockaddr_in serv_addr;
};


#endif //DNS_CLIENT_H

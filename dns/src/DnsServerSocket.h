#ifndef DNS_SERVER_SOCKET_H
#define DNS_SERVER_SOCKET_H

#include <string>

#include "DnsFwd.h"
#include "UdpSocket.h"

struct DnsServerSocket : public UdpSocket {
    using socket_t = int;
    
    explicit DnsServerSocket(int portno);
    
    void step();

private:
    socket_t client_fd;
};

#endif //DNS_SERVER_SOCKET_H

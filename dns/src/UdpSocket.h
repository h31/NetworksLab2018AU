#ifndef DNS_UDPSOCKET_H
#define DNS_UDPSOCKET_H

#include "DnsFwd.h"

struct UdpSocket {
    using socket_t = int;
    
    UdpSocket();
    
    explicit UdpSocket(const std::string &dns_server, int port=DEFAULT_UDP_PORT);
    
    void set_dns_server(const std::string &dns_server, int port=DEFAULT_UDP_PORT);
    
    virtual ~UdpSocket() = default;

protected:
    socket_t fd;
    sockaddr_in dest;
};

#endif //DNS_UDPSOCKET_H

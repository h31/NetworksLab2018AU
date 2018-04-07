#ifndef MESSENGER_DNSSERVERSOCKET_H
#define MESSENGER_DNSSERVERSOCKET_H

#include <string>

#include "DnsFwd.h"

struct DnsServerSocket {
    using socket_t = int;
    
    explicit DnsServerSocket(int portno);
    
    void step();

private:
    socket_t client_fd;
    
    socket_t dns_fd;
    
    void set_dns_server(const std::string &dns_server, int port);
    
    sockaddr_in dest;
};

#endif //MESSENGER_DNSSERVERSOCKET_H

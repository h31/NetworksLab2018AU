#ifndef MESSENGER_DNSSOCKET_H
#define MESSENGER_DNSSOCKET_H

#include <string>
#include <netinet/in.h>
#include "DnsFwd.h"

#define DEBUG_DNS 0

#define DEFAULT_UDP_PORT 53

struct DnsSocket {
    using socket_t = int;

    DnsSocket();
    
    explicit DnsSocket(const std::string &dns_server, int port=DEFAULT_UDP_PORT);
    
    void set_dns_server(const std::string &dns_server, int port=DEFAULT_UDP_PORT);
    
    std::string resolve(const std::string &hostname);

private:
    socket_t s;
    sockaddr_in dest;
};


#endif //MESSENGER_DNSSOCKET_H

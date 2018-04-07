#ifndef DNS_SOCKET_H
#define DNS_SOCKET_H

#include <string>
#include <netinet/in.h>
#include "DnsFwd.h"
#include "UdpSocket.h"

struct DnsSocket : public UdpSocket {
    using socket_t = int;

    explicit DnsSocket(const std::string &dns_server, int port);
    
    std::string resolve(const std::string &hostname);
};


#endif //DNS_SOCKET_H

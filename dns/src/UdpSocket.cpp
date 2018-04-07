#include "UdpSocket.h"
#include "DnsAll.h"

UdpSocket::UdpSocket()
        : fd(-1)
        , dest()
{}

UdpSocket::UdpSocket(const std::string &dns_server, int port) {
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    set_dns_server(dns_server, port);
}

void UdpSocket::set_dns_server(const std::string &dns_server, int port) {
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = inet_addr(dns_server.c_str());
}

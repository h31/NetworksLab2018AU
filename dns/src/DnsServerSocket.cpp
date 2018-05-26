#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstring>
#include "DnsAll.h"
#include "DnsServerSocket.h"

static std::vector<std::string> get_dns_servers() {
    std::vector<std::string> dns_servers;
    dns_servers.emplace_back("8.8.8.8");
    std::ifstream fin;
    fin.open("/etc/resolv.conf");
    if (!fin) {
        throw DnsError("Failed to open /etc/resolv.conf");
    }
    
    std::string line;
    while (!fin.eof() && std::getline(fin, line)) {
        if (line[0] == '#') {
            continue;
        }
        const std::string nameserver_prefix = "nameserver";
        if (line.substr(0, nameserver_prefix.size()) == nameserver_prefix) {
            std::istringstream is(line);
            std::string nameserver_ip;
            is >> nameserver_ip >> nameserver_ip;
            dns_servers.emplace_back(nameserver_ip);
        }
    }
    
    fin.close();
    dns_servers.emplace_back("208.67.222.222");
    dns_servers.emplace_back("208.67.220.220");
    return dns_servers;
}

DnsServerSocket::DnsServerSocket(int portno)
        : UdpSocket(get_dns_servers()[0], DEFAULT_UDP_PORT)
{
    client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_fd < 0) {
        throw DnsError("ERROR opening client socket: ");
    }
    
    sockaddr_in serv_addr{};
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    auto bind_result = bind(client_fd, reinterpret_cast<const sockaddr *>(&serv_addr), sizeof(serv_addr));
    if (bind_result < 0) {
        auto const error_msg = std::to_string(bind_result);
        throw DnsError("ERROR on binding: " + error_msg);
    }
}

void DnsServerSocket::step() {
    auto const dns_fd = fd;
    static unsigned char buf[DNS_BUF_SIZE];
    sockaddr client_dest{};
    socklen_t client_destlen;
    auto recv_result = recvfrom(client_fd, buf, DNS_BUF_SIZE, 0, &client_dest, &client_destlen);
//    assert(client_destlen == sizeof(sockaddr_in));
    if (recv_result < 0) {
        throw DnsError("recvfrom from client failed.");
    }
    
    unsigned char *qname = &buf[DNS_HEADER_SIZE];
    auto const qname_length = strlen(reinterpret_cast<const char *>(qname));
//    assert(static_cast<size_t>(recv_result) == DNS_HEADER_SIZE + (qname_length + 1) + QUESTION_SIZE);
    
    unsigned char *reader = qname;
    int stop = 0;
    
    auto hostname = read_name(reader, buf, stop);
    reader += stop;
#if DEBUG_DNS
    std::cout << "Hostname: " << hostname << std::endl;
#endif
    
    auto send_result = sendto(dns_fd,
                              buf, static_cast<size_t>(recv_result),
                              0, reinterpret_cast<const sockaddr *>(&dest), sizeof dest);
    if (send_result < 0) {
        throw DnsError("Sending to DNS server failed.");
    }
    
    socklen_t destlen;
    recv_result = recvfrom(dns_fd, buf, DNS_BUF_SIZE, 0, reinterpret_cast<sockaddr *>(&dest), &destlen);
    if (recv_result < 0) {
        throw DnsError("recvfrom from DNS server failed.");
    }
    
    send_result = sendto(client_fd, buf, static_cast<size_t>(recv_result), 0, &client_dest, client_destlen);
    if (send_result < 0) {
        throw DnsError("Sending to client failed.");
    }
}

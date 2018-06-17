#ifndef DNS_CLIENT
#define DNS_CLIENT

#pragma once

#include "../dns_packet.h"
#include <string>
#include <cerrno>
#include <arpa/inet.h>

class dns_client {
        int sock;
        sockaddr_in server_addr;
        socklen_t sizeofaddr = sizeof(server_addr);
        dns_packet create_packet(std::string& host);

public:
        dns_client(std::string const& dns_server, int port_no);

        void gethost(std::string host);
};


#endif // DNS_CLIENT



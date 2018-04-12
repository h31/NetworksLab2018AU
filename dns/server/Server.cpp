#include <arpa/inet.h>
#include <iostream>

#include "../Message.h"
#include "Server.h"

void Server::init(const std::string &hosts_file_name)
{
    resolver = new Resolver(hosts_file_name);
}

void Server::open_sockets (uint16_t port)
{
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "ERROR opening socket" << std::endl;
    }

    sockaddr_in serv_addr{};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    this->listenfd = socket_fd;
}

void Server::run()
{
    int req_size;
    char buf[PACKET_SIZE+4];
    socklen_t from_len;
    sockaddr_in from;
    dns_packet *pkt;

    from_len = sizeof (from);

    while(true)
    {
        req_size = recvfrom(this->listenfd, buf, PACKET_SIZE + 4, 0, (struct sockaddr *)&from, &from_len);

        pkt = static_cast<dns_packet *>(calloc (1, sizeof (struct dns_packet)));
        pkt->dns_request_parse(buf, req_size);

        std::string ip = resolver->get_ip(pkt->get_data());

        dns_packet* packet = static_cast<dns_packet *>(calloc (1, sizeof (struct dns_packet)));
        packet->dns_request_parse(buf, req_size);
        packet->addRR(pkt->get_data(), ip);
        int buf_len = packet->to_bytes(buf);
        sendto (this->listenfd, buf, buf_len, 0, (struct sockaddr *) &from, from_len);
    }
}

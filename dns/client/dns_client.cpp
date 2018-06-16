#include "dns_client.h"
#include <iostream>


#define T_A 1 //Ipv4 address

dns_packet dns_client::create_packet(std::string &host) {
    header dns;
    dns._flags = 0;
    dns._flags |= 1 << 8;
    dns._id = htons(1);
    dns.t_questions = 1;
    dns.t_answer_rrs = 0;
    dns.t_authority_rrs = 0;
    dns.t_additional_rrs = 0;
    query query_;
    query_.set_name(host);
    query_.type = htons(T_A);
    query_._class = htons(1);
    dns_packet packet;
    packet._header = dns;
    packet.questions.push_back(query_);
    return packet;
}

dns_client::dns_client(std::string const &dns_server, int port_no) {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = inet_addr(dns_server.c_str());
}

void dns_client::gethost(std::string host) {
    dns_packet packet = create_packet(host);
    std::cout << "Sending packet" << std::endl;
    size_t offset;
    char* buffer = packet.write_to_buf(offset);
    ssize_t sentbytes = sendto(sock, buffer, offset, 0, (sockaddr*)&server_addr, sizeof(server_addr));
    if (sentbytes <= 0) {
        std::cerr << "Unexpected send error: " + std::string(strerror(errno)) << std::endl;
    }
    memset(buffer, 0, BUF_SIZE);
    ssize_t recvbytes = recvfrom(sock, buffer, BUF_SIZE, 0, (sockaddr*)&server_addr, &sizeofaddr);
    if (recvbytes <= 0) {
        std::cout << "Unexpected receive error"  + std::string(strerror(errno)) << std::endl;
    }
    packet = dns_packet::read_from_buffer(buffer);
    packet.print_result();
    delete []buffer;
}


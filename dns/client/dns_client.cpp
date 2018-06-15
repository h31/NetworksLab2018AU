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

void dns_client::print_result(dns_packet &packet, char* buffer) {
    int count = packet._header.t_answer_rrs;
    if (packet._header._flags & 11) {
        std::cout << "The DNS server does not have this domain address" << std::endl;
    }
    for (int i = 0; i < count; i++) {
        resource_record answer = packet.answers[i];
        std::cout << "Name: " << answer.read_name(buffer) << std::endl;

        for (int j = 0; j < answer.rdata_length; j++) {
            char byte = answer._rdata[j];
            auto uns_byte = static_cast<uint8_t>(byte < 0 ? byte + 256 : byte);
            std::cout << std::to_string(uns_byte) << ".";
        }
        std::cout << std::endl;
    }
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
    char* pointer = buffer;
    packet = dns_packet::read_from_buffer(buffer);
    print_result(packet, pointer);
    delete []buffer;
}

#include "server.h"
#include "../dns_packet.h"
#include "../resource_record.h"

#include <iostream>
#include <fstream>

server::server(std::string const &hosts_list, int port) {
    std::cout << "server starting..." << std::endl;

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(client_sock, (sockaddr*)&server_addr, addrlen) < 0) {
        std::cout << "Failed to bind the address to our listening socket" << std::endl;
        exit(1);
    }

    std::ifstream in;
    in.open(hosts_list);
    if (!in.is_open()) {
        std::cout << "Failed to open file" << std::endl;
        exit(1);
    }
    std::string line;
    while(!in.eof()) {
        std::getline(in, line);
        size_t comment_start = line.find_first_of('#');
        line = line.substr(0, comment_start);
        if (line.empty()) continue;
        size_t delim = line.find_first_of(' ');
        std::string name(line, 0, delim);
        std::string ip(line, delim + 1, line.length());
        hosts.insert(std::make_pair(name, ip));
    }

    buffer = new char[BUF_SIZE];
}

server::~server() {
    delete []buffer;
}

void server::run() {
    while (true) {
        struct sockaddr_in client_address = {};
        socklen_t addr_len = sizeof(client_address);
        std::cout << " recv req " << std::endl;
        ssize_t recv_result = recvfrom(client_sock, buffer, BUF_SIZE, 0,(sockaddr*)&client_address, &addr_len);
        if (recv_result <= 0) {
            std::cerr << "Unexpected recv error: " + std::string(strerror(errno)) << std::endl;
            exit(1);
        }

        dns_packet packet = dns_packet::read_from_buffer(buffer);
        name name_ = packet.questions[0]._name;
        std::string packet_name = name_._decoded;
        if (hosts.find(packet_name) == hosts.end()) {
            packet._header._flags |= 11;
        } else {
            packet._header._flags = 0;
            packet._header._flags |= 1 << 15; // packet type response.
            packet._header.t_answer_rrs = 1;
            packet._header.t_additional_rrs = 0;
            resource_record answer = resource_record(name_);
            answer._class = 1;
            answer._type = T_A;
            answer._ttl = 200;
            answer.rdata_length = 4;
            answer._rdata = new char[4];
            std::string ip = hosts.find(packet_name)->second;
            std::cout << "ip = " << ip << std::endl;
            int ind = 0;
            for (int j = 0; j < 4; j++) {
                std::string byte = "";
                do {
                    byte += ip[ind++];
                } while (ind < ip.length() && ip[ind] != '.');
                int byte_ = atoi(byte.c_str());
                ind++;
                answer._rdata[j] = static_cast<unsigned char>(byte_);
            }
            packet.answers.clear();
            packet.answers.push_back(answer);
        }
        size_t offset;
        buffer = packet.write_to_buf(offset);
        ssize_t send_result = sendto(client_sock, buffer, offset, 0, (sockaddr*)&client_address, addr_len);
        if (send_result <= 0) {
            std::cerr << "Unexpected send error: " + std::string(strerror(errno)) << std::endl;
        }
    }
}



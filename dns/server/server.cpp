#include "server.h"
#include "../dns_packet.h"
#include "../resource_record.h"

#include <iostream>
#include <fstream>

server::server(std::string const &hosts_list) {
    std::cout << "server starting..." << std::endl;

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(53);
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
        if (line == "") break;
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
        ssize_t n = recvfrom(client_sock, buffer, BUF_SIZE, 0,(sockaddr*)&client_address, &addr_len);
        if (n == -1) {
            std::cout << "ERROR: recvfrom failed" << std::endl;
            exit(1);
        }
        dns_packet packet = dns_packet::read_from_buffer(buffer);
        if (hosts.find(packet.questions[0].name) == hosts.end()) {
            packet._header._flags |= 11;
        } else {
            packet._header._flags |= 1 << 15;
            packet._header.t_answer_rrs = 1;
            resource_record answer = resource_record();
            answer._name = sizeof(header) + 2;
            answer._class = 1;
            answer._type = 1;
            answer._ttl = 200;
            answer.rdata_length = 4;
            answer._rdata = new char[4];
            std::string ip = hosts.find(packet.questions[0].name)->second;
            int ind = 0;
            for (int j = 0; j < 4; j++) {
                std::string byte = "";
                do {
                    byte += ip[ind++];
                } while (ind < ip.length() && ip[ind] != '.');
                int byte_ = atoi(byte.c_str());
                ind++;
                answer._rdata[j] = byte_;
            }
            packet.answers = std::vector<resource_record>{answer};
        }
        size_t offset;
        buffer = packet.write_to_buf(offset);
        sendto(client_sock, buffer, offset, 0, (sockaddr*)&client_address, addr_len);
    }
}

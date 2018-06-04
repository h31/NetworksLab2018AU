#include "Client.h"

Client::Client(char *hostname, uint16_t port){
    socket_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset ((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    hostent * server = gethostbyname(hostname);
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons (port);
}

void Client::run() {
    std::string line;
    uint16_t id = 0;
    char buf[BUFFER_SIZE];
    int buf_len = 0;
    while (true) {
        std::cout << "Please, enter a domain name or 'stop' to stop the client:" << std::endl;
        std::cin >> line;
        if (line != "stop") {
            if (line.empty()) {
                continue;
            }
            dns_packet packet(id, QR_QUERY, line);
            id++;
            buf_len = packet.to_bytes(buf);
            sendto(socket_fd, buf, buf_len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
            ssize_t nbytes = recv(socket_fd, buf, 255, 0);
            auto *response = new dns_packet();
            response->dns_request_parse(buf, nbytes);
            if (response->get_id() != packet.get_id()) {
                std::cout << "Internal error" << std::endl;
            }

            if (response->header.flags & 11 == 11) {
                std::cout << "The DNS server does not have this domain address: " + line << std::endl;
            }

            for (int answ = 0; answ < response->header.ancount; answ++) {
                for (int i = 0; i < response->RRs->at(answ).rdlength; i++) {
                    char byte = response->RRs->at(answ).rdata[i];
                    auto uns_byte = static_cast<uint8_t>(byte < 0 ? byte + 256 : byte);
                    std::cout << std::to_string(uns_byte) << " ";
                }
                std::cout << std::endl;
            }
            delete response;
        } else {
            return;
        }
    }
}
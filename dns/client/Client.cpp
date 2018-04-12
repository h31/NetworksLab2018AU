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
    std::string domain_name;
    uint16_t id = 0;
    char buf[BUFFER_SIZE];
    int buf_len = 0;
    while (true) {
        std::cin >> domain_name;
        dns_packet packet(id, QR_QUERY, domain_name.data());
        id++;
        buf_len = packet.to_bytes(buf);
        sendto(socket_fd, buf, buf_len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        ssize_t nbytes = recv(socket_fd, buf, 255, 0);
        dns_packet *response;
        response = static_cast<dns_packet *>(calloc(1, sizeof(struct dns_packet)));

        response->dns_request_parse(buf, nbytes);
        if (response->get_id() != packet.get_id()) {
            std::cerr << "Wrong id of response message" << std::endl;
        }
        
        for (int i = 0; i < response->RRs->at(0).rdlength; i++) {
            std::cout <<  response->RRs->at(0).rdata[i] + 128 << " ";
        }
        std::cout << std::endl;
    }
}
//
// Created by kate on 26.03.18.
//

// источник: https://www.binarytides.com/dns-query-code-in-c-with-winsock/

#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>

#include "../common/common.h"


const static int PORT = 53;
char buffer[MAX_UDP_SIZE];


int main(int argc, char **argv) {
    int server_id = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_id < 0) {
        std::cout << "ERROR: open socket" << std::endl;
        exit(1);
    }
    const int optval = 1;
    setsockopt(server_id, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    struct sockaddr_in addr = {};
    memset((char *) &addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if (bind(server_id, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        std::cout << "ERROR: bind failed" << std::endl;
        exit(1);
    }

    while (true) {
        struct sockaddr_in client_address = {};

        socklen_t addr_len = sizeof(client_address);

        std::cout << " recv req " << std::endl;
        ssize_t n = recvfrom(server_id, buffer, MAX_UDP_SIZE, 0,
                             (struct sockaddr *) &client_address, &addr_len);
        if (n == -1) {
            std::cout << "ERROR: recvfrom failed" << std::endl;
            exit(1);
        }

        auto *dns = (struct DNS_HEADER *) buffer;
        auto *qname = buffer + sizeof(struct DNS_HEADER);
        dns->qr = 1;
        dns->ans_count = htons(1);
        char *reader = buffer + sizeof(struct DNS_HEADER) + strlen(qname) + 1 +
                sizeof(struct QUESTION);

        memcpy(reader, qname, strlen(qname) + 1);
        reader += strlen(qname) + 1;

        auto *data = (struct R_DATA *) reader;
        data->type = htons(1);
        data->ttl = htonl(200);
        data->_class = htons(1);
        data->data_len = htons(4);
        reader += sizeof(struct R_DATA);
        *(int *) reader = htonl(static_cast<uint32_t>(strlen(qname)));
        n = 2 * strlen((char *) qname) + 2 + sizeof(struct R_DATA)
            + sizeof(struct DNS_HEADER) + sizeof(struct QUESTION) + 4;


        struct hostent *hostp = gethostbyaddr(
                (char *) &client_address.sin_addr.s_addr,
                sizeof client_address.sin_addr.s_addr, AF_INET);
        if (hostp == nullptr) {
            std::cout << "ERROR: gethostbyaddr" << std::endl;
            exit(1);
        }

        char *hostaddrp = inet_ntoa(client_address.sin_addr);
        if (hostaddrp == nullptr) {
            std::cout << "ERROR: inet_ntoa" << std::endl;
            exit(1);
        }

        std::cout << "send packet" << std::endl;

        if (sendto(server_id, buffer, static_cast<size_t>(n), 0,
                   (struct sockaddr *) &client_address, addr_len) == -1) {
            std::cout << "ERROR: sendto" << std::endl;
            exit(1);
        }
    }
    return 0;
}

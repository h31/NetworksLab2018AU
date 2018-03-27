//
// Created by kate on 26.03.18.
//

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>    //getpid

#include "../common/common.h"

const static int PORT = 1234;

char buffer[MAX_UDP_SIZE];


int unpack() {
    struct DNS_HEADER *dns = (struct DNS_HEADER *) buffer;
    char *qname = (char *) (buffer + DNS_HEADER_SIZE);
    dns->qr = 1;
    dns->ans_count = htons(1);
    char *reader = buffer + DNS_HEADER_SIZE +
                   strlen((const char *) qname) + 1 + QUESTION_SIZE;

    memcpy(reader, qname, strlen((char *) qname) + 1);
    reader += strlen((char *) qname) + 1;

    struct R_DATA *data = (struct R_DATA *) reader;
    data->type = htons(1);
    data->ttl = htonl(200);
    data->_class = htons(1);
    data->data_len = htons(4);
//    reader += R_DATA_SIZE;
    *(int *) (reader + R_DATA_SIZE) = htonl(strlen((char *) qname));
    return strlen((char *) qname) + 1 + R_DATA_SIZE + 4;
}

int main(int argc, char **argv) {
    int server_id = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_id < 0) {
        std::cout << "ERROR: open socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in addr;
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
        struct sockaddr_in client_address;

        socklen_t addr_len = sizeof(addr);

        std::cout << " recv req " << std::endl;
        if (recvfrom(server_id, buffer, MAX_UDP_SIZE, 0,
                     (struct sockaddr *) &addr, &addr_len) == -1) {
            std::cout << "ERROR: recvfrom failed" << std::endl;
            exit(1);
        }

        int n = unpack();

        struct hostent *hostp = gethostbyaddr(
                (char *) &client_address.sin_addr.s_addr,
                sizeof client_address.sin_addr.s_addr, AF_INET);
        if (hostp  == nullptr) {
            std::cout << "ERROR: gethostbyaddr" << std::endl;
            exit(1);
        }

        char *hostaddrp = inet_ntoa(client_address.sin_addr);
        if (hostaddrp == nullptr) {
            std::cout << "ERROR: inet_ntoa" << std::endl;
            exit(1);
        }

        printf("Sending packet");

        if (sendto(server_id, buffer, n, 0, (struct sockaddr *) &addr,
                   addr_len) == -1) {
            std::cout << "ERROR: sendto" << std::endl;
        }
    }
}

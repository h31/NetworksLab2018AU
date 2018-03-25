#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <memory.h>
#include <message.h>
#include "message.h"

void get_options(int argc, char** argv, char** dns_server_addr, uint16_t* dns_server_port);

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "usage: -a <dns-server-addr> -p <dns-server-port>");
        exit(1);
    }
    uint16_t dns_server_port;
    char *dns_server_addr;
    get_options(argc, argv, &dns_server_addr, &dns_server_port);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    char server_port[6];
    snprintf(server_port, 6, "%u", dns_server_port);

    if (getaddrinfo(dns_server_addr, server_port, &hints, &res) != 0) {
        perror("ERROR: getaddrinfo failed");
        return 1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    while (1) {
        question_t question;
        char *name = NULL;
        size_t name_length = 0;
        printf("input domain name: ");
        ssize_t count = getline(&name, &name_length, stdin);
        if (count < 0) {
            perror("ERROR: Cannot read domain name");
            exit(-1);
        }
        // remove newline
        --count;
        name[count] = '\0';
        // first and last bytes are busy
        if (count > NAME_LENGTH - 2) {
            fprintf(stderr, "domain name must be less then 253");
            exit(-1);
        }
        uint8_t pos = 0;
        uint8_t index = 0;
        for (index = 0; index < count; ++index) {
            if (name[index] == '.') {
                question.name[pos] = index - pos;
                pos = index + (uint8_t) 1;
            } else {
                question.name[index + 1] = name[index];
            }
        }
        question.name[pos] = index - pos;
        question.name[index + 1] = 0;

        // IN  1 the Internet
        question.class = CLASS;
        question.type = TYPE;

        uint16_t id = 0;
        header_t header;
        header.id = id;
        header.qr = 0;
        header.opcode = 0;
        header.rd = 1;
        header.z = 0;
        header.qdcount = 1;
        header.ancount = 0;
        header.nscount = 0;
        header.arcount = 0;

        ssize_t n = sendto(sockfd, &header, sizeof(header_t), 0,
                           res->ai_addr, res->ai_addrlen);
        if (n < 0) {
            perror("failed to send header to server\n");
            exit(-1);
        }

        n = sendto(sockfd, &question, sizeof(question_t), 0, res->ai_addr, res->ai_addrlen);
        if (n < 0) {
            perror("failed to send question to server\n");
            exit(-1);
        }

        answer_t answer;
        struct sockaddr from_sock;
        socklen_t socklen = sizeof(from_sock);
        n = recvfrom(sockfd, &header, sizeof(header_t), 0, &from_sock, &socklen);
        if (n < 0) {
            perror("failed to receive header from server\n");
            continue;
        }

        if (header.rcode != 0) {
            fprintf(stderr, "failed with code %d\n", header.rcode);
            continue;
        }
        n = recvfrom(sockfd, &answer, sizeof(answer_t), 0, &from_sock, &socklen);
        if (n < 0) {
            perror("failed to receive answer from server\n");
            continue;
        }
        printf("IPv4 of %s: %d.%d.%d.%d\n", name, answer.rdata[0], answer.rdata[1],
               answer.rdata[2], answer.rdata[3]);
        free(name);
    }
    return 0;
}
#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <message.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "message.h"

#define DEFAULT_TTL 100

void get_options(int argc, char** argv, uint16_t* port);
int main(int argc, char **argv) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;     // Fill IP auto

    uint16_t portno;
    get_options(argc, argv, &portno);
    char server_port[6];
    snprintf(server_port, 6, "%u", portno);

    if (getaddrinfo(NULL, server_port, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        return 1;
    }
    int sockfd;
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("ERROR: Could not create socket");
        return 1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("ERROR on binding");
        return 1;
    }

    while (1) {
        struct sockaddr client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        header_t header;
        ssize_t count = recvfrom(sockfd, &header, sizeof(header_t), 0,
                                 &client_addr, &client_addr_len);
        if (count < 0) {
            fprintf(stderr, "failed to receive header from client\n");
            continue;
        }
        question_t question;
        count = recvfrom(sockfd, &question, sizeof(question_t), 0,
                         &client_addr, &client_addr_len);
        if (count < 0) {
            fprintf(stderr, "failed to receive question from client\n");
            continue;
        }
        // answer
        struct addrinfo *answer_addr;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;

        char addr[NAME_LENGTH];
        uint8_t length = (uint8_t)(question.name[0] & 0x3f);
        uint8_t start = 0;
        while (length != 0) {
            for (uint8_t index = 0; index < length; ++index) {
                addr[start + index] = question.name[start + index + 1];
            }
            start += length;
            if ((length = (uint8_t)(question.name[start + 1] & 0x3f)) != 0) {
                addr[start] = '.';
            } else {
                addr[start] = '\0';
            }
            ++start;
        }

        int addr_res = getaddrinfo(addr, NULL, &hints, &answer_addr);
        uint8_t rcode;
        switch (addr_res) {
            case 0:
                rcode = 0;
                break;
            case EAI_AGAIN:
                rcode = 2;
                break;
            case EAI_BADFLAGS:
                rcode = 1;
                break;
            case EAI_FAIL:
                rcode = 2;
                break;
            case EAI_FAMILY:
                rcode = 4;
                break;
            case EAI_MEMORY:
                rcode = 2;
                break;
            case EAI_NONAME:
                rcode = 1;
                break;
            case EAI_SERVICE:
                rcode = 3;
                break;
            case EAI_SOCKTYPE:
                rcode = 1;
                break;
            case EAI_SYSTEM:
                rcode = 2;
                break;
            default:
                rcode = 2;
                break;
        }
        header.qr = 1;
        header.ra = header.rd;
        header.rcode = rcode;
        header.qdcount = 0;
        if (header.rcode != 0) {
            header.ancount = 0;
        } else {
            header.ancount = 1;
        }
        count = sendto(sockfd, &header, sizeof(header_t), 0, &client_addr, client_addr_len);
        if (count < 0) {
            perror("ERROR: send to client");
            continue;
        }

        if (header.ancount == 0) {
            continue;
        }

        answer_t answer;
        memcpy(answer.name, question.name, NAME_LENGTH);
        answer.type = TYPE;
        answer.class = CLASS;
        answer.ttl = DEFAULT_TTL;
        answer.rdlength = 4;
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)answer_addr->ai_addr;
        char* temp = inet_ntoa(ipv4->sin_addr);
        uint8_t octet = 0;
        while (*temp != '\0') {
            uint8_t n = (uint8_t)(atoi(temp));
            while (*temp != '\0' && *temp != '.') {
                ++temp;
            }
            if (*temp == '.') {
                ++temp;
            }
            answer.rdata[octet] = n;
            octet += 1;
        }
        count = sendto(sockfd, &answer, sizeof(answer_t), 0, &client_addr, client_addr_len);
        if (count < 0) {
            perror("ERROR: send to client\n");
            continue;
        }
    }
}
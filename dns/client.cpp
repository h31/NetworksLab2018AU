#include <arpa/inet.h>
#include <iostream>
#include "common.h"

int main(int argc, char *argv[]) {
    DnsRequest dnsRequest;
    DnsHeader dnsHeader;
    DnsQuery dnsQuery;
    DnsResponse dnsResponse;
    char answer_name[MAX_MSG_LEN];

    dnsRequest.header = &dnsHeader;
    dnsRequest.query = &dnsQuery;
    dnsResponse.header = &dnsHeader;
    dnsResponse.query = &dnsQuery;
    dnsResponse.name = answer_name;
    dnsRequest.header->id = 0x0000;
    dnsRequest.header->flags = 0x0100;
    dnsRequest.header->questions = 0x0001;
    dnsRequest.header->answerRRs = 0x0000;
    dnsRequest.header->autorityRRs = 0x0000;
    dnsRequest.header->additionalRRs = 0x0000;
    dnsRequest.query->type = 0x0001;
    dnsRequest.query->clazz = 0x0001;

    if (argc < 3 || argc > 4) {
        std::cout << "Wrong number of arguments\n Usage: client <dns-server> <domain> <?dns-port>\n";
        return 0;
    }
    uint16_t portno = (argc == 3 ? DNS_PORT : atoi(argv[3]));

    char* dns_server = argv[1];
    char* domain = argv[2];
    dnsRequest.query->domain = domain;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket opening failed\n";
        return 1;
    }

    sockaddr_in serv_addr;
    socklen_t serv_addr_sz = sizeof(serv_addr);
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(dns_server);
    serv_addr.sin_port = htons(portno);

    uint8_t buffer[MAX_MSG_LEN];
    bzero(buffer, MAX_MSG_LEN);

    int query_len = writeDnsRequest(dnsRequest, buffer);
    int sent = sendto(sockfd, buffer, query_len, 0, (sockaddr*) &serv_addr, serv_addr_sz);
    if (sent < 0) {
        std::cerr << "Sending failed\n";
        close(sockfd);
        return 1;
    }
    bzero(buffer, MAX_MSG_LEN);
    recvfrom(sockfd, buffer, MAX_MSG_LEN, 0, (sockaddr*) &serv_addr, &serv_addr_sz);

    readDnsResponse(dnsResponse, buffer);

    std::cout << "Name: " << dnsResponse.name <<"\n";
    std::cout << "TTL : " << dnsResponse.ttl << "\n";
    std::cout << "IP  : " << std::to_string(dnsResponse.ip[0]) << "." << std::to_string(dnsResponse.ip[1]) << "."
                          << std::to_string(dnsResponse.ip[2]) << "." << std::to_string(dnsResponse.ip[3]) << "\n";
    close(sockfd);
    return 0;
}
#include <iostream>
#include "common.h"


int main(int argc, char *argv[]) {
    DnsRequest dnsRequest;
    DnsResponse dnsResponse;
    DnsHeader dnsHeader;
    DnsQuery dnsQuery;
    char domain[MAX_MSG_LEN];
    dnsRequest.header = &dnsHeader;
    dnsRequest.query = &dnsQuery;
    dnsQuery.domain = domain;
    dnsResponse.name = domain;
    dnsResponse.header = &dnsHeader;
    dnsResponse.query = &dnsQuery;
    dnsResponse.ttl = TTL;
    dnsResponse.ip[0] = 127;
    dnsResponse.ip[1] = 0;
    dnsResponse.ip[2] = 0;
    dnsResponse.ip[3] = 1;
    dnsResponse.dataLen = 4;


    uint16_t portno = DNS_PORT;
    if (argc == 2) {
        portno = atoi(argv[1]);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        std::cerr << "Socket opening failed\n";
        return 1;
    }

    sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Binding failed\n";
        return 1;
    }

    sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    uint8_t buffer[MAX_MSG_LEN];

    while (true) {
        bzero(buffer, MAX_MSG_LEN);
        recvfrom(sockfd, buffer, MAX_MSG_LEN, 0,(sockaddr *) &cli_addr, &cli_len);
        int query_len = readDnsRequest(dnsRequest, buffer);
        dnsResponse.header->flags = 0x8180;
        dnsResponse.header->answerRRs = 1;
        dnsResponse.type = dnsRequest.query->type;
        dnsResponse.clazz = dnsRequest.query->type;
        writeDnsResponse(dnsResponse, buffer);

        int n = sendto(sockfd, buffer, query_len + 16, 0,(sockaddr *) &cli_addr, cli_len);
        if (n < 0) {
            std::cerr << "Sending failed\n";
        }
    }
}
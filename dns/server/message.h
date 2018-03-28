#include <cstdint>

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

struct header {
    uint16_t ID;
    uint16_t QR:1;
    uint16_t Opcode:4;
    uint16_t AA:1;
    uint16_t TC:1;
    uint16_t RD:1;
    uint16_t RA:1;
    uint16_t Z:3;
    uint16_t RCODE:4;
    uint16_t QDCOUNT;
    uint16_t ANCOUNT;
    uint16_t NSCOUNT;
    uint16_t ARCOUNT;
};

struct dns_question {
    uint16_t QTYPE;
    uint16_t QCLASS;
};

struct dns_rdata {
    uint16_t TYPE;
    uint16_t CLASS;
    uint16_t TTL;
    uint16_t RDLENGTH;
};

struct query {
    char* QNAME;
    dns_question* question;
};

struct rr {
    char* NAME;
    dns_rdata* res;
    char* RDATA;
};

#endif //SERVER_MESSAGE_H
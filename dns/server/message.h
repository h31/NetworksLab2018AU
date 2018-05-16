#include <cstdint>

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

struct header {
    uint16_t ID;
    uint8_t RD:1;
    uint8_t TC:1;
    uint8_t AA:1;
    uint8_t Opcode:4;
    uint8_t QR:1;
    uint8_t RCODE:4;
    uint8_t Z:3;
    uint8_t RA:1;

    uint16_t QDCOUNT;
    uint16_t ANCOUNT;
    uint16_t NSCOUNT;
    uint16_t ARCOUNT;
};

struct dns_question {
    uint16_t QTYPE;
    uint16_t QCLASS;
};

#pragma pack(push, 1)
struct dns_rdata {
    uint16_t TYPE;
    uint16_t CLASS;
    uint32_t TTL;
    uint16_t RDLENGTH;
};
#pragma pack(pop)

#endif //SERVER_MESSAGE_H
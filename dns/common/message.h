#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include <stdint.h>
#define NAME_LENGTH 255
// IN 1
#define CLASS 1
// TYPE A
#define TYPE 255

typedef struct {
    uint16_t id;
    uint16_t qr:1;
    uint16_t opcode:4;
    uint16_t aa:1;
    uint16_t tc:1;
    uint16_t rd:1;
    uint16_t ra:1;
    uint16_t z:3;
    uint16_t rcode:4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} header_t;

typedef struct {
    char name[NAME_LENGTH];
    uint16_t type;
    uint16_t class;
} question_t;

typedef struct {
    char name[NAME_LENGTH];
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    // 32 bit, because we only support class A, therefore only IPv4 which 4 bytes
    uint8_t rdata[4];
} answer_t;

#endif //CLIENT_MESSAGE_H
